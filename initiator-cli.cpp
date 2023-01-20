#include "initiator.hpp"
#include <iostream>

#include <fcntl.h>	// File control definitions
#include <errno.h>	// errno
#include <string.h>	// strerror
#include <iomanip>	// setw()
#include <utility>

using namespace std;
using namespace hwitl;

template<typename T>
struct HexPrint {
	HexPrint(T a) : a(a){};
	T a;
};

template<typename T>
std::ostream& operator<<( std::ostream& o, const HexPrint<T>& a ){
	const auto flags = o.flags();
	o << "0x";
	o << setfill('.');
	o << setw( sizeof(T) * 2);
	o << hex;
	o << a.a;
	o.setf(flags);
	return o;
}

void readWriteAtZero(Initiator& remote) {
	constexpr Address address = 0x00000000;
	constexpr Payload payload = 0xFF;

	cout << "[Initiator] write " << HexPrint{address} << " value " << payload << endl;
	auto stat = remote.write(address, payload);
	if(stat != ResponseStatus::Ack::ok) {
		cerr << "[Initiator] Nak on write: " << static_cast<unsigned>(stat) << endl;
		return;
	}
	cout << "[Initiator] read " << HexPrint{address} << " ";
	auto ret = remote.read(address);
	if(ret.status.ack != ResponseStatus::Ack::ok) {
		cerr << "[Initiator] Nak on read: "  << static_cast<unsigned>(ret.status.ack) << endl;
		return;
	}
	cout << "[Initiator] value " << HexPrint{ret.payload} << endl;
	if(remote.getInterrupt())
		cout << "[Initiator] Interrupt was triggered" << endl;
	sleep(1);
}

void sweepAddressRoom(Initiator& remote) {
	constexpr Address startAddr = 0x00000000;
	constexpr Address   endAddr = 0x0000FFFF;
	constexpr Payload payload = 0x8BADF00D;
	constexpr Address printStep = 0x00001000;
	for(Address address = startAddr; address <= endAddr; address += sizeof(Payload)) {
		if(address % printStep == 0) {
			cout << "[Initiator] Scanning " << HexPrint{address} << " - " << HexPrint{(address + printStep)} << " ..." << endl;
		}
		auto ret = remote.read(address);
		if(ret.status.ack == ResponseStatus::Ack::not_mapped) {
			// nothing
		} else if(ret.status.ack == ResponseStatus::Ack::ok) {
			cout << "[Initiator] \t" << "Found Register at " << HexPrint{address} << " : " << HexPrint{ret.payload} << endl;
		} else {
			cout << "[Initiator] Other error at " << HexPrint{address} << ": [" << static_cast<int>(ret.status.ack) << "]" << endl;
		}
	}
}

int main(int argc, char* argv[]) {
	int handle = -1;
	if (argc > 1){
		handle = open(argv[1], O_RDWR| O_NOCTTY);
	}
	if (handle < 0) {
		cerr << "[Initiator] autsch: " << strerror(errno) << endl;
		return -1;
	}

	Initiator initiator(handle);

	//while(handle) {
		// different modes
		sweepAddressRoom(initiator);
	//}

	close(handle);
	cerr << "[Initiator] end" << endl;
	return 0;
}
