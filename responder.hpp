#pragma once
#include "protocol.hpp"
#include "magic.hpp"
#include <fstream>

#include <functional>
#include <list>

class Responder {
public:
	struct AddressRange{
		hwitl::Address from;
		hwitl::Address to;
	};
	typedef std::function<hwitl::Payload(hwitl::Address)> ReadCallback;
	typedef std::function<void(hwitl::Address,hwitl::Payload)> WriteCallback;
	struct CallbackEntry{
		AddressRange range;
		ReadCallback read;
		WriteCallback write;
	};
private:
	std::fstream& m_handle;
	bool irq_active;
	std::list<CallbackEntry> registeredRanges;
	CallbackEntry getRegisteredCallback(hwitl::Address);
public:
	Responder(std::fstream& handle) : m_handle(handle), irq_active(false){};

	void addCallback(CallbackEntry);
	void listener();
	void setIRQ();
};
