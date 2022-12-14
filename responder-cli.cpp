#include "responder.hpp"
#include <iostream>
#include <fcntl.h>	// File control definitions
#include <errno.h>	// errno
#include <string.h>	// strerror
#include <thread>

using namespace std;
using namespace hwitl;

Payload genericReadCallback(Address address) {
	cout << "[responder-cli] [Callback] read 0x" << hex << address << dec << endl;
	static char b = 'A';
	return b++;
}

void genericWriteCallback(Address address, Payload payload) {
	cout << "[responder-cli] [Callback] write 0x" << hex << address << " value " << payload << dec << endl;
}

void generateInterrupts(Responder& responder){
	bool on = true;
	while(true) {
		if(on)
			cout << "[responder-cli] triggered interrupt" << endl;
		responder.setIRQ(on);
		sleep(1);
		on = !on;
	}
}

int main(int argc, char* argv[]) {
	int handle = -1;
	if (argc > 1){
		handle = open(argv[1], O_RDWR| O_NOCTTY);
	}
	if (handle < 0) {
		cerr << "autsch: " << strerror(errno) << endl;
		return -1;
	}

	Responder responder(handle);

	responder.addCallback(Responder::CallbackEntry{
			.range = Responder::AddressRange{.from = 0x0000, .to = 0xFFFFFFFF},
			.read = bind(genericReadCallback, placeholders::_1),
			.write = bind(genericWriteCallback, placeholders::_1, placeholders::_2)}
	);

	thread listener = thread(&Responder::listener, &responder);
	thread interruptor = thread([&responder](){generateInterrupts(responder);});

	if(listener.joinable())
		listener.join();

	// don't care for interruptor thread

	cerr << "end" << endl;
	return 0;
}
