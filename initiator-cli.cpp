#include "initiator.hpp"
#include <iostream>

#include <fcntl.h>	// File control definitions
#include <errno.h>	// errno
#include <string.h>	// strerror

using namespace std;
using namespace hwitl;

void readWriteAtZero(Initiator& remote) {
	constexpr Address address = 0x00000000;
	constexpr Payload payload = 0xFF;

	cout << "write 0x" << hex << address << " value " << payload << dec << endl;
	auto stat = remote.write(address, payload);
	if(stat != ResponseStatus::Ack::ok) {
		cerr << "Nak on write: " << static_cast<unsigned>(stat) << endl;
		return;
	}
	cout << "read 0x" << hex << address << dec << " ";
	auto ret = remote.read(address);
	if(ret.status.ack != ResponseStatus::Ack::ok) {
		cerr << "Nak on read: "  << static_cast<unsigned>(ret.status.ack) << endl;
		return;
	}
	cout << "value 0x" << hex << ret.payload << dec << endl;
	if(remote.getInterrupt())
		cout << "Interrupt was triggered" << endl;
	sleep(1);
}

void sweepAddressRoom(Initiator& remote) {
	constexpr Address maxAddr = 0x0000FFFF;
	constexpr Payload payload = 0x8BADF00D;
	constexpr Address printStep = 0x00001000;
	for(Address address = 0x00000000; address <= maxAddr; address += sizeof(Payload)) {
		if(address % printStep == 0) {
			cout << hex << "Scanning " << "0x" << address << " - 0x" << (address + printStep) << " ..." << endl;
		}
		auto ret = remote.read(address);
		if(ret.status.ack == ResponseStatus::Ack::not_mapped) {
			// nothing
		} else if(ret.status.ack == ResponseStatus::Ack::ok) {
			cout << hex << "\t" << "Found Register at 0x" << address << " : 0x" << ret.payload << endl;
		} else {
			cerr << hex << "Other error at 0x" << address << ": [0x" << ret.status.ack << "]";
		}
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

	Initiator initiator(handle);

	while(handle) {
		// different modes
		sweepAddressRoom(initiator);
	}

	cerr << "end" << endl;
	return 0;
}
