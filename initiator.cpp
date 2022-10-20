#include <iostream>
#include "initiator.hpp"
#include <fstream>

#include <fcntl.h>	// File control definitions
#include <errno.h>	// errno
#include <string.h>	// strerror

using namespace std;
using namespace hwitl;

ResponseRead Initiator::read(Address address) {
	Request req {Request::Command::read, address};
	if(!writeStruct(m_handle, req))
		cerr << "[Initiator read] Error transmitting request" << endl;
	ResponseRead res{};
	if(!readStruct(m_handle, res))
		cerr << "[Initiator read] Error reading response" << endl;

	is_irq_waiting = res.status.irq_waiting;
	return res;
}
ResponseStatus::Ack Initiator::write(Address address, Payload pl) {
	Request req {Request::Command::write, address};
	if(!writeStruct(m_handle, req))
		cerr << "[Initiator write] Error transmitting request" << endl;
	if(!writeStruct(m_handle, pl))
		cerr << "[Initiator write] Error transmitting payload" << endl;
	ResponseWrite res{};
	if(!readStruct(m_handle, res))
		cerr << "[Initiator write] Error reading response" << endl;
	is_irq_waiting = res.status.irq_waiting;
	return res.status.ack;
}

bool Initiator::getIRQstatus() {
	return is_irq_waiting;
}

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
