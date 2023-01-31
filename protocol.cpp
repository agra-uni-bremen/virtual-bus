#include "protocol.hpp"
#include <arpa/inet.h>	// htonl
#include "network_io.hpp"


hwitl::Request::Request(const Command cmd, const Address addr)
		: command(cmd) {
	static_assert(sizeof(Address) == 4 && "Need different endian conversion function");
	address = htonl(addr);	// Here is the network order conversion
};

hwitl::Request::Command hwitl::Request::getCommand() const {
	return command;
}

hwitl::Address hwitl::Request::getAddressToHost() const {
	return ntohl(address);	// Network to Host conversion
}

hwitl::RequestRead::RequestRead(const Address addr) : request(Request::Command::read, addr){};

hwitl::RequestWrite::RequestWrite(const Address addr, const Payload pl)
		: request(Request::Command::write, addr) {
	payload = pl;	// No network order conversion
};

hwitl::RequestIRQ::RequestIRQ()
		: request(Request::Command::getIRQ, 0){};	// Zero Address, others reserved for future use


hwitl::ResponseStatus::ResponseStatus(const Ack status, const bool is_irq_waiting)
		: ack(status), irq_waiting(is_irq_waiting){};
