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
	std::cout << std::hex << addr << " <- " << val << std::endl;
}

int main() {
	std::string output;
	auto writer = [&output](const char* str) { output += str; };
	intel_hex::write_all(writer, peek, 0x1000, 256);

	auto reader = [&output, pos{0}]() mutable -> char { return output[pos++]; };
	intel_hex::read(reader, poke);
}