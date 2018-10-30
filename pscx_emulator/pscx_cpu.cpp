#include "pscx_cpu.h"

#include <iostream>
#include <cassert>

uint32_t Cpu::load32(uint32_t addr)
{
	return m_inter.load32(addr);
}

void Cpu::store32(uint32_t addr, uint32_t value)
{
	m_inter.store32(addr, value);
}

Cpu::InstructionType Cpu::decodeAndExecute(Instruction instruction)
{
	InstructionType instructionType = INSTRUCTION_TYPE_UNKNOWN;

	switch (instruction.getInstructionCode())
	{
	case /*LUI*/0b001111:
		instructionType = opcodeLUI(instruction);
		break;
	case /*ORI*/0b001101:
		instructionType = opcodeORI(instruction);
		break;
	case /*SW*/0b101011:
		instructionType = opcodeSW(instruction);
		break;
	default:
		std::cout << "unhandled instruction 0x" << std::hex << instruction.getInstructionCode() << std::endl;
	}

	return instructionType;
}

//----------------------------------------------
// TODO : to implement the runNextInstruction function.
// It should fetch an instruction at program counter (PC) register,
// increment PC to point to the next instruction and
// execute instruction.
Cpu::InstructionType Cpu::runNextInstuction()
{ 
	// Fixme
	Instruction instruction = Cpu::load32(Cpu::m_pc);
	Cpu::m_pc = Cpu::m_pc + 4;
	return Cpu::decodeAndExecute(instruction);
}

//--------------------------------------------------------------
// TODO : to implement the Load Upper Immediate function (LUI).
// It should load the immediate value into the high 16 bits of the target register.
Cpu::InstructionType Cpu::opcodeLUI(Instruction instruction)
{
	// Fixme
	setRegisterValue(instruction.getRegisterTargetIndex(), instruction.getImmediateValue() << 16);

	return INSTRUCTION_TYPE_LUI;
}

Cpu::InstructionType Cpu::opcodeORI(Instruction instruction)
{
	setRegisterValue(instruction.getRegisterTargetIndex(), instruction.getImmediateValue() | getRegisterValue(instruction.getRegisterSourceIndex()));
	return INSTRUCTION_TYPE_ORI;
}

Cpu::InstructionType Cpu::opcodeSW(Instruction instruction)
{
	uint32_t registerSourceIndex = instruction.getRegisterSourceIndex();
	uint32_t registerTargetIndex = instruction.getRegisterTargetIndex();
	store32(getRegisterValue(registerSourceIndex) + instruction.getSignExtendedImmediateValue(), getRegisterValue(registerTargetIndex));
	return INSTRUCTION_TYPE_SW;
}

uint32_t Cpu::getRegisterValue(uint32_t index) const
{
	assert(index < _countof(m_regs), "Index out of boundaries");
	return m_regs[index];
}

void Cpu::setRegisterValue(uint32_t index, uint32_t value)
{
	assert(index < _countof(m_regs), "Index out of boundaries");
	if (index > 0) m_regs[index] = value;
}