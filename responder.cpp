#include "responder.hpp"
#include <iostream>
#include <errno.h>	// errno
#include <string.h>	// strerror

using namespace std;
using namespace hwitl;

Responder::CallbackEntry Responder::getRegisteredCallback(hwitl::Address target) {
	//cout << hex << "Callback target: 0x" << target << endl;
	for(const auto& entry : registeredRanges) {
		//cout << "from 0x" << hex << entry.range.from << " to 0x" << entry.range.to << dec << endl;
		if(entry.range.from <= target && entry.range.to > target) {
			return entry;
		}
	}
	//cout << "Callback not found" << endl;
	return Responder::CallbackEntry{{0,0},nullptr,nullptr};
}
void Responder::addCallback(CallbackEntry callback) {
	if(!isAddressRangeValid(callback.range)) {
		cerr << "Callback bad" << endl;
		return;
	}
	registeredRanges.emplace_front(callback);
}
void Responder::listener() {
	cout << "[responder] listening" << endl;
	while(m_handle) {
		Request req;
		if(!readStruct(m_handle, req)) {
			cerr << "[responder] error reading request" << endl;
			return;
		}
		const auto targetAddress = req.getAddressToHost();
		auto callback = getRegisteredCallback(targetAddress);
		const bool is_mapped = isAddressRangeValid(callback.range);
		ResponseStatus stat(ResponseStatus::Ack::ok, irq_active);
		switch(req.getCommand()) {
		case Request::Command::read:
		{
			Payload payload = 0;
			if(!is_mapped) {
				cerr << "Callback on address 0x" << hex << targetAddress << dec << " not mapped" << endl;
				stat.ack = ResponseStatus::Ack::not_mapped;
			} else {
				if(!callback.read) {
					cerr << "Callback on address 0x" << hex << targetAddress << dec << " not readable" << endl;
					stat.ack = ResponseStatus::Ack::command_not_supported;
				} else {
					payload = callback.read(targetAddress);
				}
			}
			ResponseRead response{stat, payload};
			if(!writeStruct(m_handle, response)) {
				cerr << "[responder] error writing response read" << strerror(errno) << endl;
			}
		}
		break;
		case Request::Command::write:
		{
			Payload payload;
			if(!readStruct(m_handle, payload)){
				cerr << "[responder] error reading payload" << strerror(errno) << endl;
			}
			if(!is_mapped) {
				cerr << "Callback on address 0x" << hex << targetAddress << dec << " not mapped" << endl;
				stat.ack = ResponseStatus::Ack::not_mapped;
			} else {
				if(!callback.write) {
					cerr << "Callback on address 0x" << hex << targetAddress << dec << " not writable" << endl;
					stat.ack = ResponseStatus::Ack::command_not_supported;
				} else {
					callback.write(targetAddress, payload);
				}
			}
			ResponseWrite response{stat};
			if(!writeStruct(m_handle, response)) {
				cerr << "[responder] error writing response write" << strerror(errno) << endl;
			}
		}
		break;
		case Request::Command::getIRQ:
			if(!writeStruct(m_handle, stat)) {
				cerr << "[responder] error writing keepalive" << strerror(errno) << endl;
			}
			break;
		case Request::Command::exit:
			cerr << "graceful exit" << endl;
			return;
		default:
		{
			stat.ack = ResponseStatus::Ack::command_not_supported;
			ResponseWrite response{stat};
			if(!writeStruct(m_handle, response)) {
				cerr << "[responder] error writing response write" << strerror(errno) << endl;
			}
		}
			return;
		}
	}
}
void Responder::setIRQ(bool active) {
	irq_active = active;
}

