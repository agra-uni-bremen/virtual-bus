#include <iostream>
#include "initiator.hpp"

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