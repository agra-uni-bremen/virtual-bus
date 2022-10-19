/*
 * magic.hpp
 *
 *  Created on: Oct 19, 2022
 *      Author: dwd
 */

#pragma once
#include <fstream>

template<typename T>
void writeStruct(std::fstream& handle, T& pl) {
	handle.write(reinterpret_cast<char*>(&pl), sizeof(T));
}

template<typename T>
void readStruct(std::fstream& handle, T& pl) {
	handle.read(reinterpret_cast<char*>(&pl), sizeof(T));
}
