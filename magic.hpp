/*
 * magic.hpp
 *
 *  Created on: Oct 19, 2022
 *      Author: dwd
 */

#pragma once
#include <unistd.h>

template<typename T>
int writeStruct(int handle, T& pl) {
	return write(handle, reinterpret_cast<char*>(&pl), sizeof(T));
}

template<typename T>
bool readStruct(int handle, T& pl) {
	return read(handle, reinterpret_cast<char*>(&pl), sizeof(T));
}
