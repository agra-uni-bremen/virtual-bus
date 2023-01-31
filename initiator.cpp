#include <iostream>
#include "initiator.hpp"

using namespace std;
using namespace hwitl;

ResponseRead Initiator::read(Address address) {
	RequestRead req (address);
	ResponseRead res;
	if(!writeStruct(m_handle, req)) {
		cerr << "[Initiator read] Error transmitting read request" << endl;
		res.status.ack = ResponseStatus::Ack::never;
		return res;
	}

	if(!readStruct(m_handle, res)) {
		cerr << "[Initiator read] Error reading read response" << endl;
		res.status.ack = ResponseStatus::Ack::never;
		return res;
	}

	if(res.status.ack == ResponseStatus::Ack::ok)
		is_irq_waiting = res.status.irq_waiting;
	return res;
}
ResponseStatus::Ack Initiator::write(Address address, Payload pl) {
	RequestWrite req(address, pl);
	if(!writeStruct(m_handle, req)) {
		cerr << "[Initiator write] Error transmitting write request" << endl;
		return ResponseStatus::Ack::never;
	}
	ResponseWrite res;
	if(!readStruct(m_handle, res)) {
		cerr << "[Initiator write] Error reading response" << endl;
		return ResponseStatus::Ack::never;
	}
	if(res.status.ack == ResponseStatus::Ack::ok)
		is_irq_waiting = res.status.irq_waiting;
	return res.status.ack;
}

ResponseStatus::Ack Initiator::update() {
	RequestIRQ req;
	if(!writeStruct(m_handle, req))
		cerr << "[Initiator read] Error transmitting update request" << endl;
	ResponseStatus res;
	if(!readStruct(m_handle, res))
		cerr << "[Initiator read] Error reading update response" << endl;
	if(res.ack == ResponseStatus::Ack::ok)
		is_irq_waiting = res.irq_waiting;
	return res.ack;
}

bool Initiator::getInterrupt() {
	if(!is_irq_waiting) {
		update();	// is getIRQ necessary?
	}
	return is_irq_waiting;
}
