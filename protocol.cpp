#include "protocol.hpp"
#include <arpa/inet.h>	// htonl
#include "network_io.hpp"


hwitl::Request::Request(const Command command, const Address address)
		: m_command(command) {
	static_assert(sizeof(Address) == 4 && "Need different endian conversion function");
	m_address = htonl(address);
};
hwitl::Request hwitl::Request::fromNetworkOrder(const Command raw_command, const Address raw_address) {
	return Request{raw_command, raw_address};
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

hwitl::RequestWrite::RequestWrite(const Address network_addr, const Payload network_payload) {
	request = Request::fromNetworkOrder(Request::Command::read, network_addr);
	payload = network_payload;
};

hwitl::Payload hwitl::RequestWrite::getPayload() const{
	static_assert(sizeof(Payload) == 4 && "Need different endian conversion function");
	return ntohl(payload);
}

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

