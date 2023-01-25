// FIXME: Is this a nice place?
// Rationale: "Protocol" because this defines network order on protocol
#include "protocol.hpp"
#include "magic.hpp"
#include <arpa/inet.h>

template<>
int writeStruct(int handle, hwitl::ResponseRead& pl) {
	static_assert(sizeof(hwitl::ResponseStatus) == 1,
			"ResponseStatus now also needs endianess correction");
	pl.payload = htonl(pl.payload);
	return write(handle, reinterpret_cast<char*>(&pl), sizeof(hwitl::ResponseRead));
}

template<>
bool readStruct(int handle, hwitl::ResponseRead& pl) {
	int ret = read(handle, reinterpret_cast<char*>(&pl), sizeof(hwitl::ResponseRead));
	if(ret > 0){	// if successful
		pl.payload = ntohl(pl.payload);
	}
	return ret;
}

template<>
int writeStruct(int handle, hwitl::Request& pl) {
	static_assert(sizeof(hwitl::Request::Command) == 1,
			"Request::Command now also needs endianess correction");
	pl.address = htonl(pl.address);
	return write(handle, reinterpret_cast<char*>(&pl), sizeof(hwitl::Request));
}

template<>
bool readStruct(int handle, hwitl::Request& pl) {
	int ret = read(handle, reinterpret_cast<char*>(&pl), sizeof(hwitl::Request));
	if(ret > 0){	// if successful
		pl.address = ntohl(pl.address);
	}
	return ret;
}

template<>
int writeStruct(int handle, hwitl::Payload& pl) {
	static_assert(sizeof(hwitl::Payload) == 4 && "Need different endian conversion function");
	pl = htonl(pl);
	return write(handle, reinterpret_cast<char*>(&pl), sizeof(hwitl::Payload));
}


