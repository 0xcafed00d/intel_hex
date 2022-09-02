#ifndef INTEL_HEX_H_0xCAFED00D
#define INTEL_HEX_H_0xCAFED00D

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
		bool read_byte(fileReader& r, std::uint8_t& val, std::uint8_t& chk_tot) {
			char buffer[3];
			buffer[0] = r();
			buffer[1] = r();
			buffer[2] = 0;

			if (!std::isxdigit(buffer[0]) || !std::isxdigit(buffer[1])) {
				return false;
			}

			val = strtol(buffer, nullptr, 16);
			chk_tot += val;
			return true;
		}

		template <typename fileReader>
		bool read_16bit(fileReader& r, std::uint16_t& val, std::uint8_t& chk_tot) {
			std::uint8_t hi, lo;
			bool ok = true;
			ok &= read_byte(r, hi, chk_tot);
			ok &= read_byte(r, lo, chk_tot);
			val = std::uint16_t(hi) << 8 | lo;
			return ok;
		}
	}  // namespace helpers

	template <typename fileReader, typename memWriter>
	bool read_line(fileReader& r, memWriter& w, bool& endrecord) {
		using namespace helpers;
		char c = r();
		while (std::isspace(c)) {  // skip past any white space
			c = r();
		}

		if (c != ':') {
			return false;
		}

		std::uint8_t chk_tot = 0;
		std::uint8_t len, type, data, check;
		std::uint16_t addr;

		bool ok = true;
		ok &= read_byte(r, len, chk_tot);
		ok &= read_16bit(r, addr, chk_tot);
		ok &= read_byte(r, type, chk_tot);
		ok &= (type == 0) || (type == 1);

		while (len-- && ok) {
			ok &= read_byte(r, data, chk_tot);
			w(addr++, data);
		}
		ok &= read_byte(r, check, chk_tot);
		ok &= (chk_tot == 0);

		endrecord = (type == 1);

		return ok;
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

		bool endrecord = false;
		bool ok = true;

		while (ok) {
			ok &= read_line(r, w, endrecord);
		}
		return ok;
	}

}  // namespace intel_hex

#endif /* INTEL_HEX_H_0xCAFED00D */
