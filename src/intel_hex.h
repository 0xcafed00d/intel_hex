#ifndef BC45DBCC_061E_4977_B2B2_0C1D57025446
#define BC45DBCC_061E_4977_B2B2_0C1D57025446

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <exception>
#include <type_traits>

// fileWriter -> void write(const char* str) -- throws on error
// fileReader -> char read() -- throws on error - returns 0 on eof
// memReader -> std::uint8_t peek(std::unit16_t address)
// memWriter -> void poke(std::unit16_t address, std::uint8_t val)

// intel hex format:
// :LLAAAATTDD..DDCC
// LL   = length data
// AAAA = 16bit address
// TT   = record type
//		  00 = data record
//        01 = end of file record
//        currently all other record types are not supported.
// DD   = Data bytes
// CC   = 8 bit checksum: 2's complement of 8bit sum of all record bytes

namespace intel_hex {
	namespace helpers {

		template <typename fileWriter>
		void write_byte(fileWriter& w, std::uint8_t val) {
			char buffer[3];
			sprintf(buffer, "%02x", val);
			w(buffer);
		}

		template <typename fileWriter>
		void write_word(fileWriter& w, std::uint16_t val) {
			write_byte(w, val >> 8);
			write_byte(w, val);
		}
	}  // namespace helpers

	// ---------------------------------------------------------------------------------------
	// Writing intel hex files ....
	// ---------------------------------------------------------------------------------------

	template <typename fileWriter, typename memReader>
	void write_data(fileWriter w, memReader& r, std::uint16_t address, size_t len) {
		using namespace helpers;

		static_assert(std::is_invocable_v<fileWriter, const char*>,
		              "fileWriter must be invocable as: "
		              "void fileWriter(const char* str)");

		static_assert(std::is_invocable_r_v<std::uint8_t, memReader, std::uint16_t>,
		              "memReader must be invocable as: "
		              "std::uint8_t memReader(std::uint16_t adress)");

		while (len) {
			size_t line_len = std::min(size_t(16), len);
			len -= line_len;

			std::uint8_t check = line_len + (address & 0xff) + ((address >> 8) & 0xff);

			w(":");
			write_byte(w, line_len);
			write_word(w, address);
			w("00");
			while (line_len--) {
				auto val = r(address);
				check += val;
				write_byte(w, val);
				address++;
			}
			write_byte(w, (~check) + 1);
			w("\n");
		}
	}

	template <typename fileWriter>
	void write_end(fileWriter& w) {
		w(":00000001FF");
	}

	template <typename fileWriter, typename memReader>
	void write_all(fileWriter& w, memReader& r, std::uint16_t address, size_t len) {
		write_data(w, r, address, len);
		write_end(w);
	}

	// ---------------------------------------------------------------------------------------
	// Reading intel hex files ....
	// ---------------------------------------------------------------------------------------
	namespace helpers {
		template <typename fileReader>
		bool read_byte(fileReader& r, std::uint8_t& val) {
			char buffer[3];
			buffer[0] = r();
			buffer[1] = r();
			buffer[2] = 0;

			if (!std::isxdigit(buffer[0]) || !std::isxdigit(buffer[1])) {
				return false;
			}

			val = strtol(buffer, std::nullptr_t, 16);
			return true;
		}
	}  // namespace helpers

	template <typename fileReader, typename memWriter>
	bool read_line(fileReader& r, memWriter& w) {
		char c = r();
		if (c != ':') {
			return false;
		}
	}

	template <typename fileReader, typename memWriter>
	bool read(fileReader& r, memWriter& w) {
		using namespace helpers;

		static_assert(std::is_invocable_r_v<char, fileReader>,
		              "fileReader must be invocable as: "
		              "char fileWriter()");

		static_assert(std::is_invocable_v<memWriter, std::uint16_t, std::uint8_t>,
		              "memReader must be invocable as: "
		              "void memWriter(std::uint16_t address, std::uint8_t val)");
	}

}  // namespace intel_hex

#endif /* BC45DBCC_061E_4977_B2B2_0C1D57025446 */
