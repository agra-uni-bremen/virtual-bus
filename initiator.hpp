#pragma once
#include "protocol.hpp"
#include <fstream>
#include "network_io.hpp"

class Initiator {
	int m_handle;
	bool is_irq_waiting;
public:
	Initiator(int& handle) : m_handle(handle), is_irq_waiting(false){};

	hwitl::ResponseRead read(hwitl::Address pl);
	hwitl::ResponseStatus::Ack write(hwitl::Address address, hwitl::Payload pl);
	hwitl::ResponseStatus::Ack update();
	bool getInterrupt();
};
