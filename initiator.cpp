#include <iostream>
#include "initiator.hpp"
#include <fstream>

using namespace std;
using namespace hwitl;

ResponseRead Initiator::read(Address address) {
	Request req {Request::Command::read, address};
	writeStruct(m_handle, req);
	ResponseRead res{};
	readStruct(m_handle, res);
	is_irq_waiting = res.status.irq_waiting;
	return res;
}
ResponseStatus::Ack Initiator::write(Address address, Payload pl) {
	Request req {Request::Command::write, address};
	writeStruct(m_handle, req);
	writeStruct(m_handle, pl);
	ResponseWrite res{};
	readStruct(m_handle, res);
	is_irq_waiting = res.status.irq_waiting;
	return res.status.ack;
}

bool Initiator::getIRQstatus() {
	return is_irq_waiting;
}

int main(int argc, char* argv[]) {
	std::fstream handle;
	if (argc > 1){
		handle.open(argv[1]);
	}
	if (!handle.is_open()) {
		cerr << "autsch" << endl;
		return -1;
	}

	Initiator initiator(handle);

	while(true) {
		constexpr Address address = 0x00000000;
		constexpr Payload payload = 0xFF;

		cout << "write 0x" << hex << address << " value " << payload << dec << endl;
		auto stat = initiator.write(0x00000000, 0xFF);
		if(stat != ResponseStatus::Ack::ok) {
			cerr << "Nak on write" << endl;
			break;
		}
		cout << "read 0x" << hex << address << dec;
		auto ret = initiator.read(0x00000000);
		if(ret.status.ack != ResponseStatus::Ack::ok) {
			cerr << "Nak on read" << endl;
			break;
		}
		cout << hex << ret.payload << dec << endl;

	}

	cerr << "end" << endl;
	return 0;
}
