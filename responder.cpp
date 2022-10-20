#include <iostream>
#include "responder.hpp"
#include <fcntl.h>	// File control definitions
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
	return Responder::CallbackEntry{{0,0}};
}
void Responder::addCallback(CallbackEntry callback) {
	if(!isAddressRangeValid(callback.range)) {
		cerr << "Callback bad" << endl;
		return;
	}
	registeredRanges.emplace_front(callback);
}
void Responder::listener() {
	cout << "listening" << endl;
	while(m_handle) {
		Request req;
		if(!readStruct(m_handle, req)) {
			cerr << "[responder] error reading request" << endl;
			return;
		}
		auto callback = getRegisteredCallback(req.address);
		const bool is_mapped = isAddressRangeValid(callback.range);
		switch(req.command) {
		case Request::Command::read:
		{
			ResponseStatus stat{irq_active, ResponseStatus::Ack::ok};
			Payload payload = 0;
			if(!is_mapped) {
    			cerr << "Callback on address 0x" << hex << req.address << dec << " not mapped" << endl;
				stat.ack = ResponseStatus::Ack::not_mapped;
			} else {
				if(!callback.read) {
    				cerr << "Callback on address 0x" << hex << req.address << dec << " not readable" << endl;
					stat.ack = ResponseStatus::Ack::other_problem;
				} else {
					payload = callback.read(req.address);
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
			ResponseStatus stat{irq_active, ResponseStatus::Ack::ok};
			Payload payload;
			if(!readStruct(m_handle, payload)){
				cerr << "[responder] error reading payload" << strerror(errno) << endl;
			}
			if(!is_mapped) {
    			cerr << "Callback on address 0x" << hex << req.address << dec << " not mapped" << endl;
				stat.ack = ResponseStatus::Ack::not_mapped;
			} else {
				if(!callback.write) {
					cerr << "Callback on address 0x" << hex << req.address << dec << " not writable" << endl;
					stat.ack = ResponseStatus::Ack::other_problem;
				} else {
					callback.write(req.address, payload);
				}
			}
			ResponseWrite response{stat};
			if(!writeStruct(m_handle, response)) {
				cerr << "[responder] error writing response write" << strerror(errno) << endl;
			}
		}
		break;
		default:
			cerr << "auerhahn" << endl;
			return;
		}
	}
}
void Responder::setIRQ() {
	irq_active = true;
}




Payload genericReadCallback(Address address) {
	cout << "[Callback] read 0x" << hex << address << dec << endl;
	return 0x1337;
}

void genericWriteCallback(Address address, Payload payload) {
	cout << "[Callback] write 0x" << hex << address << " value " << payload << dec << endl;
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

	Responder responder(handle);

	responder.addCallback(Responder::CallbackEntry{
			.range = Responder::AddressRange{.from = 0x0000, .to = 0xFFFF},
			.read = bind(genericReadCallback, placeholders::_1),
			.write = bind(genericWriteCallback, placeholders::_1, placeholders::_2)}
	);

	responder.listener();

	cerr << "end" << endl;
	return 0;
}
