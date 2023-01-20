/*
 * magic.hpp
 *
 *  Created on: Oct 19, 2022
 *      Author: dwd
 */

#pragma once
#include "protocol.hpp"
#include <unistd.h>

template<typename T>
int writeStruct(int handle, T& pl) {
	return write(handle, reinterpret_cast<char*>(&pl), sizeof(T));
}

template<typename T>
bool readStruct(int handle, T& pl) {
	return read(handle, reinterpret_cast<char*>(&pl), sizeof(T));
}

template<>
int writeStruct(int handle, hwitl::ResponseRead& pl);

template<>
bool readStruct(int handle, hwitl::ResponseRead& pl);

template<>
int writeStruct(int handle, hwitl::Request& pl);

template<>
bool readStruct(int handle, hwitl::Request& pl);
