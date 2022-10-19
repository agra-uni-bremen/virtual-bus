#include <iostream>
#include "responder.hpp"
#include <fstream>

using namespace std;
using namespace hwitl;

Responder::CallbackEntry Responder::getRegisteredCallback(hwitl::Address target) {
	for(const auto& entry : registeredRanges) {
		if(entry.range.from <= target && entry.range.to > target) {
			return entry;
		}
	}
	return Responder::CallbackEntry{{0,0}};
}
void Responder::addCallback(CallbackEntry callback) {
	if(callback.range.from == 0 && callback.range.to <= callback.range.from) {
		cerr << "Callback bad" << endl;
	}
	registeredRanges.emplace_front(callback);
}
void Responder::listener() {
	cout << "listening" << endl;
	while(m_handle.is_open()) {
		Request req;
		readStruct(m_handle, req);
		auto callback = getRegisteredCallback(req.address);
		const bool is_mapped = callback.range.from != 0 && callback.range.to != 0;
		switch(req.command) {
		case Request::Command::read:
		{
			ResponseStatus stat{irq_active, ResponseStatus::Ack::ok};
			Payload payload = 0;
			if(!is_mapped) {
				stat.ack = ResponseStatus::Ack::not_mapped;
			} else {
				if(!callback.read) {
					stat.ack = ResponseStatus::Ack::other_problem;
				} else {
					payload = callback.read(req.address);
				}
			}
			ResponseRead response{stat, payload};
			writeStruct(m_handle, response);
		}
		break;
		case Request::Command::write:
		{
			ResponseStatus stat{irq_active, ResponseStatus::Ack::ok};
			Payload payload;
			readStruct(m_handle, payload);
			if(!is_mapped) {
				stat.ack = ResponseStatus::Ack::not_mapped;
			} else {
				if(!callback.write) {
					stat.ack = ResponseStatus::Ack::other_problem;
				} else {
					callback.write(req.address, payload);
				}
			}
			ResponseWrite response{stat};
			writeStruct(m_handle, response);
		}
		break;
		default:
			cerr << "auerhahn" << endl;
		}
	}
}
void Responder::setIRQ() {
	irq_active = true;
}




Payload genericReadCallback(Address address) {
	cout << "read 0x" << hex << address << dec << endl;
	return 0x1337;
}

void genericWriteCallback(Address address, Payload payload) {
	cout << "write 0x" << hex << address << " value " << payload << dec << endl;
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
