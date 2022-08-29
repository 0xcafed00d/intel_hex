#include <iostream>

#include "intel_hex.h"

void writer (std::string_view s){
    std::cout << s << std::endl;
}

std::uint8_t peek(std::uint16_t addr){
    return addr;
}

int main() {

    intel_hex::write_data(writer, peek, 0x1000, 256);
    intel_hex::write_end(writer);

}
