#include "pscx_bios.h"

#include <fstream>
#include <iterator>
#include <iostream>

using namespace std;
// load the BIOS binary file into array of bytes u8 (m_data).
Bios::BiosState Bios::loadBios(std::string path)
{
		//The file is open with the ios::ate flag, which means that the get pointer will be positioned at the end of the file.
		ifstream input(path, ios::binary | ios::ate);
	
		if (input.is_open()) {

			//Call tellg() to get the size of the file (pointer is at the end of the file).
			int size = input.tellg();
			if (size == 512 * 1024) {
				
				int s = sizeof(uint8_t);
				vector<uint8_t> m_data(size/s);

				for (int count = 0; size > s; count++, size = size - s) {

					input.seekg(count * s, ios::beg);
					uint8_t &val = m_data[count];
					input.read(reinterpret_cast<char*> (&val), s);
					m_data[count] = val;

				}
				
				Bios::m_data = m_data;

				input.close();
				return BIOS_STATE_SUCCESS;
			}
			else {
				input.close();
				return BIOS_STATE_INVALID_BIOS_SIZE;
			}
		}
		else {
			return BIOS_STATE_INCORRECT_FILENAME;
		}
}

uint32_t Bios::load32(uint32_t offset) const
{
	uint32_t b0 = m_data[offset + 0];
	uint32_t b1 = m_data[offset + 1];
	uint32_t b2 = m_data[offset + 2];
	uint32_t b3 = m_data[offset + 3];

	return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

uint8_t Bios::load8(uint32_t offset) const
{
	return m_data[offset];
}