/*
 * magic.hpp
 *
 *  Created on: Oct 19, 2022
 *      Author: dwd
 */

#include "network_io.hpp"
#include <fcntl.h>
#include <inttypes.h>

bool clearInputBuffer(int handle) {
	int flags = fcntl(handle, F_GETFL, 0);
	if(fcntl(handle, F_SETFL, flags | O_NONBLOCK) < 0)
		return false;

	uint8_t dummy;
	while(read(handle, &dummy, 1) >= 1) {};

	// restore file flags
	if(fcntl(handle, F_SETFL, flags) < 0)
		return false;

	return true;
}
