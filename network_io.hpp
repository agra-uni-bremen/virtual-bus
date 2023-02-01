/*
 * magic.hpp
 *
 *  Created on: Oct 19, 2022
 *      Author: dwd
 */

#pragma once
#include <unistd.h>

template<typename T>
bool writeStruct(int handle, T& pl) {
	return write(handle, reinterpret_cast<char*>(&pl), sizeof(T)) == sizeof(T);
}

template<typename T>
bool readStruct(int handle, T& pl) {
	return read(handle, reinterpret_cast<char*>(&pl), sizeof(T)) == sizeof(T);
}

bool clearInputBuffer(int handle);
