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
