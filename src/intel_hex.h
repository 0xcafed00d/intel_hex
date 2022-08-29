#ifndef BC45DBCC_061E_4977_B2B2_0C1D57025446
#define BC45DBCC_061E_4977_B2B2_0C1D57025446

#include <string_view>
#include <cstdint>
#include <algorithm>

// fileWriter -> void write(std::string_view s) -- throws on error
// memReader -> std::uint8_t peek(std::unit16_t address)

namespace intel_hex{

    template <typename memReader>
    std::uint8_t calc_checksum(memReader& r, std::uint16_t address, size_t len){
        std::uint16_t tot = 0;
        
        while (len--){
            tot += r(address);
            address++;
        }

        return std::uint8_t(-tot);
    }

    template <typename fileWriter>
    void write_byte (fileWriter& w, std::uint8_t b){
        char buffer[3];
        sprintf(buffer, "%02x", b);
        w(buffer);
    }

    template <typename fileWriter>
    void write_word (fileWriter& w, std::uint16_t w){
        char buffer[5];
        sprintf(buffer, "%04x", w);
        w(buffer);
    }

    template <typename fileWriter, typename memReader>
    void write_data (fileWriter& w, memReader& r, std::uint16_t address, size_t len){        
        while (len){
            size_t line_len = std::min<size_t, size_t>(16, len);
            len -= line_len;

            std::uint8_t check = calc_checksum(r, address, len); 
            
            w(":");
            write_byte(w, line_len);
            write_word(w, address);
            w("00");
            write_byte(w, check);
            w("\n");
        }
    }

    template <typename fileWriter>
    void write_end (fileWriter& w){
        w(":00000001FF");
    }
}

#endif /* BC45DBCC_061E_4977_B2B2_0C1D57025446 */
