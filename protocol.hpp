/*
 * protocol.hpp
 *
 *  Created on: Oct 19, 2022
 *      Author: dwd
 */

#pragma once

#include <inttypes.h>

namespace hwitl {
typedef uint32_t Address;
typedef uint32_t Payload;

struct __attribute__((packed)) Request {
	enum class Command : uint8_t {
		reset = 0,
		read = 1,
		write,
		getIRQ,
		setTime,
		exit
	} command;
	Address address;
};

struct __attribute__((packed)) ResponseStatus {
	/*
	 * packed struct starts from LSB to MSB
	 * Ack: bits 0 to 6
	 * irq_waiting: bit 7
	 */
	enum class Ack : uint8_t {
		never = 0,
		ok = 1,
		not_mapped,
		command_not_supported
	} ack : 7;
	bool irq_waiting : 1;
};

struct ResponseRead {
	ResponseStatus status;
	Payload payload;
};

struct ResponseWrite {
	ResponseStatus status;
};

}
