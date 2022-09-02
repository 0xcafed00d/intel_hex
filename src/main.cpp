#include <iostream>
#include <string_view>

#include "intel_hex.h"

void writer(std::string_view s) {
	std::cout << s;
}

std::uint8_t peek(std::uint16_t addr) {
	return addr;
}

void poke(std::uint16_t addr, std::uint8_t val) {
	std::cout << " poke: " << std::hex << addr << " <- " << int(val) << std::endl;
}

int main() {
	std::string output;
	auto writer = [&output](const char* str) {
		output += str;
		std::cout << str;
	};
	intel_hex::write_all(writer, peek, 0x1000, 200);

	std::cout << " -----------------------------------------" << std::endl;

	auto reader = [&output, pos{0}]() mutable -> char {
		std::cout << output[pos];
		return output[pos++];
	};
	return !intel_hex::read(reader, poke);
}