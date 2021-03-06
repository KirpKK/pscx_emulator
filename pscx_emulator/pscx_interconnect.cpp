#include "pscx_interconnect.h"

#include <iostream>

Interconnect::Interconnect(Bios bios) :
	m_bios(bios)
{
}

Instruction Interconnect::load32(uint32_t addr) const
{
	uint32_t targetPeripheralAddress = maskRegion(addr);

	uint32_t offset = 0;
	if (addr & 0b11)
	{
		LOG("Unaligned load32 address 0x" << std::hex << addr);
		return Instruction(~0, Instruction::INSTRUCTION_STATUS_UNALIGNED_ACCESS);
	}

	if (BIOS.contains(targetPeripheralAddress, offset))
		return Instruction(m_bios.load32(offset));

	if (RAM.contains(targetPeripheralAddress, offset))
		return Instruction(m_ram.load32(offset));

	if (MEM_CONTROL.contains(targetPeripheralAddress, offset))
		return Instruction(~0, Instruction::INSTRUCTION_STATUS_NOT_IMPLEMENTED);

	if (SPU.contains(targetPeripheralAddress, offset))
		return Instruction(~0, Instruction::INSTRUCTION_STATUS_NOT_IMPLEMENTED);

	if (RAM_SIZE.contains(targetPeripheralAddress, offset))
		return Instruction(~0, Instruction::INSTRUCTION_STATUS_NOT_IMPLEMENTED);

	if (EXPANSION_1.contains(targetPeripheralAddress, offset))
		return Instruction(~0, Instruction::INSTRUCTION_STATUS_NOT_IMPLEMENTED);

	if (EXPANSION_2.contains(targetPeripheralAddress, offset))
		return Instruction(~0, Instruction::INSTRUCTION_STATUS_NOT_IMPLEMENTED);
	
	LOG("Unhandled fetch32 at address 0x" << std::hex << addr);
	return Instruction(~0, Instruction::INSTRUCTION_STATUS_UNHANDLED_FETCH);
}

Instruction Interconnect::load8(uint32_t addr) const
{
	uint32_t targetPeripheralAddress = maskRegion(addr);

	uint32_t offset = 0;
	if (BIOS.contains(targetPeripheralAddress, offset))
		return Instruction(m_bios.load8(offset));

	if (EXPANSION_1.contains(targetPeripheralAddress, offset))
	{
		// No expansion implemented
		return Instruction(0xff);
	}

	LOG("Unhandled load8 at address 0x" << std::hex << addr);
	return Instruction(~0, Instruction::INSTRUCTION_STATUS_UNHANDLED_FETCH);
}

void Interconnect::store32(uint32_t addr, uint32_t value)
{
	if (addr & 0b11)
	{
		LOG("Unaligned store32 address 0x" << std::hex << addr);
		return;
	}

	uint32_t targetPeripheralAddress = maskRegion(addr);

	uint32_t offset = 0;
	if (MEM_CONTROL.contains(targetPeripheralAddress, offset))
	{
		switch (offset)
		{
		case 0:
		{
			// Expansion 1 base address
			if (value != 0x1f000000)
			{
				LOG("Bad expansion 1 base address 0x" << std::hex << value);
				return;
			}
			break;
		}
		case 4:
		{
			// Expansion 2 base address
			if (value != 0x1f802000)
			{
				LOG("Bad expansion 2 base address 0x" << std::hex << value);
				return;
			}
			break;
		}
		default:
			LOG("Unhandled write to MEM_CONTROL register");
			return;
		}
	}

	if (RAM.contains(targetPeripheralAddress, offset))
	{
		m_ram.store32(offset, value);
	}

	if (RAM_SIZE.contains(targetPeripheralAddress, offset))
	{
		LOG("Ignore store to RAM_SIZE register for now");
		return;
	}

	if (CACHE_CONTROL.contains(targetPeripheralAddress, offset))
	{
		LOG("Ignore store to CACHE_CONTROL register for now");
		return;
	}

	LOG("Unhandled store32 into address 0x" << std::hex << targetPeripheralAddress);
}

void Interconnect::store16(uint32_t addr, uint16_t value)
{
	if (addr & 1)
	{
		LOG("Unaligned store16 address 0x" << std::hex << addr);
		return;
	}

	uint32_t targetPeripheralAddress = maskRegion(addr);

	uint32_t offset = 0;
	if (SPU.contains(targetPeripheralAddress, offset))
	{
		LOG("Unaligned write to SPU register " << offset);
		return;
	}

	LOG("Unhandled store16 into address 0x" << std::hex << targetPeripheralAddress);
}

void Interconnect::store8(uint32_t addr, uint8_t value)
{
	uint32_t targetPeripheralAddress = maskRegion(addr);

	uint32_t offset = 0;
	if (EXPANSION_2.contains(targetPeripheralAddress, offset))
	{
		LOG("Unaligned write to expansion 2 register " << offset);
		return;
	}

	LOG("Unhandled store8 into address 0x" << std::hex << targetPeripheralAddress);
}
