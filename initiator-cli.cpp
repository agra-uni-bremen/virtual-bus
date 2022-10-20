#include "initiator.hpp"
#include <iostream>

#include <fcntl.h>	// File control definitions
#include <errno.h>	// errno
#include <string.h>	// strerror

using namespace std;
using namespace hwitl;

int main(int argc, char* argv[]) {
	int handle;
	if (argc > 1){
		handle = open(argv[1], O_RDWR| O_NOCTTY);
		// todo set tty stuff
	}
	if (!handle) {
		cerr << "autsch " << strerror(errno) << endl;
		return -1;
	}

	Initiator initiator(handle);

	while(handle) {
		constexpr Address address = 0x00000000;
		constexpr Payload payload = 0xFF;

		cout << "write 0x" << hex << address << " value " << payload << dec << endl;
		auto stat = initiator.write(address, payload);
		if(stat != ResponseStatus::Ack::ok) {
			cerr << "Nak on write: " << static_cast<unsigned>(stat) << endl;
			break;
		}
		cout << "read 0x" << hex << address << dec << " ";
		auto ret = initiator.read(address);
		if(ret.status.ack != ResponseStatus::Ack::ok) {
			cerr << "Nak on read: "  << static_cast<unsigned>(ret.status.ack) << endl;
			break;
		}
		cout << "value 0x" << hex << ret.payload << dec << endl;
		sleep(1);
	}

	cerr << "end" << endl;
	return 0;
}
