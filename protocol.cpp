#include "protocol.hpp"
#include <arpa/inet.h>	// htonl
#include "network_io.hpp"


hwitl::Request::Request(const Command command, const Address address)
		: m_command(command) {
	static_assert(sizeof(Address) == 4 && "Need different endian conversion function");
	m_address = htonl(address);
};

hwitl::Request::Command hwitl::Request::getCommand() const {
	return m_command;
}

hwitl::Address hwitl::Request::getAddressToHost() const {
	static_assert(sizeof(Address) == 4 && "Need different endian conversion function");
	return ntohl(m_address);	// Network to Host conversion
}

hwitl::RequestRead::RequestRead(const Address addr)
		: request(Request::Command::read, addr){};

hwitl::RequestWrite::RequestWrite(const Address addr, const Payload pl)
		: request(Request::Command::write, addr) {
	static_assert(sizeof(Payload) == 4 && "Need different endian conversion function");
	payload = htonl(pl);
};

hwitl::Payload hwitl::RequestWrite::fromNetwork(const Payload pl) {
	static_assert(sizeof(Payload) == 4 && "Need different endian conversion function");
	return ntohl(pl);
}

hwitl::RequestIRQ::RequestIRQ()
		: request(Request::Command::getIRQ, 0){};	// Zero Address, others reserved for future use

hwitl::ResponseStatus::ResponseStatus(const Ack status, const bool is_irq_waiting)
		: ack(status), irq_waiting(is_irq_waiting){};

hwitl::ResponseRead::ResponseRead(const ResponseStatus status, const Payload payload)
		: m_status(status){
	m_payload = htonl(payload);
}

hwitl::ResponseStatus hwitl::ResponseRead::getStatus() const {
	return m_status;
}

hwitl::Payload hwitl::ResponseRead::getPayload() const {
	return ntohl(m_payload);
}

