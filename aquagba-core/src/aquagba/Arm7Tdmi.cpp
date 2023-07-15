#include "aquagba/Arm7Tdmi.hpp"
#include "aquagba/bus.hpp"
#include "aquagba/util.hpp"
#include "fmt/format.h"

using namespace aquagba;

Arm7Tdmi::Arm7Tdmi()
{
    Reset();
}

void Arm7Tdmi::Reset()
{
    // Write old PC to r14_svc
    WriteRegisterDirect(RegisterName::r15, ReadRegisterDirect(RegisterName::r14_svc));

    // Write old PSR to spsr_svc
    WriteRegisterDirect(RegisterName::spsr_svc, ReadRegisterDirect(RegisterName::cpsr));

    // Enter supervisor mode
    mCurrentMode = ProcessorMode::Supervisor;

    // Set irq and fiq disable bits
    mCurrentPsr.irq_disable = true;
    mCurrentPsr.fiq_disable = true;

    // Reset PC to 0
    WriteRegisterDirect(RegisterName::r15, 0);

    // Switch to ARM mode
    mCurrentPsr.state = OperatingState::Arm;
}

void Arm7Tdmi::WriteRegisterDirect(RegisterName reg, uint32_t value)
{
    switch (reg)
    {
    case RegisterName::cpsr:
        mCurrentPsr = ProgramStatus::FromBinary(value);
        break;
    case RegisterName::spsr_fiq:
        mSavedPsrs[ProcessorMode::Fiq] = ProgramStatus::FromBinary(value);
        break;
    case RegisterName::spsr_svc:
        mSavedPsrs[ProcessorMode::Supervisor] = ProgramStatus::FromBinary(value);
        break;
    case RegisterName::spsr_abt:
        mSavedPsrs[ProcessorMode::Abort] = ProgramStatus::FromBinary(value);
        break;
    case RegisterName::spsr_irq:
        mSavedPsrs[ProcessorMode::Irq] = ProgramStatus::FromBinary(value);
        break;
    case RegisterName::spsr_und:
        mSavedPsrs[ProcessorMode::Undefined] = ProgramStatus::FromBinary(value);
        break;
    default:
         mRegisters[static_cast<int>(MapRegister(reg))] = value;
         break;
    }
}

uint32_t Arm7Tdmi::ReadRegisterDirect(RegisterName reg)
{
    switch (reg)
    {
    case RegisterName::cpsr:
        return mCurrentPsr.AsBinary();
    case RegisterName::spsr_fiq:
        return mSavedPsrs[ProcessorMode::Fiq].AsBinary();
    case RegisterName::spsr_svc:
        return mSavedPsrs[ProcessorMode::Supervisor].AsBinary();
    case RegisterName::spsr_abt:
        return mSavedPsrs[ProcessorMode::Abort].AsBinary();
    case RegisterName::spsr_irq:
        return mSavedPsrs[ProcessorMode::Irq].AsBinary();
    case RegisterName::spsr_und:
        return mSavedPsrs[ProcessorMode::Undefined].AsBinary();
    default:
         return mRegisters[static_cast<int>(MapRegister(reg))];
    }
}

RegisterName Arm7Tdmi::MapRegister(RegisterName reg)
{
    RegisterName real_reg;

    // Map thumb registers to real registers
    switch (reg)
    {
    case RegisterName::sp:
        real_reg = RegisterName::r13;
        break;
    case RegisterName::sp_fiq:
        real_reg = RegisterName::r13_fiq;
        break;
    case RegisterName::sp_svc:
        real_reg = RegisterName::r13_svc;
        break;
    case RegisterName::sp_abt:
        real_reg = RegisterName::r13_abt;
        break;
    case RegisterName::sp_irq:
        real_reg = RegisterName::r13_irq;
        break;
    case RegisterName::sp_und:
        real_reg = RegisterName::r13_und;
        break;
    case RegisterName::lr:
        real_reg = RegisterName::r14;
        break;
    case RegisterName::lr_fiq:
        real_reg = RegisterName::r14_fiq;
        break;
    case RegisterName::lr_svc:
        real_reg = RegisterName::r14_svc;
        break;
    case RegisterName::lr_abt:
        real_reg = RegisterName::r14_abt;
        break;
    case RegisterName::lr_irq:
        real_reg = RegisterName::r14_irq;
        break;
    case RegisterName::lr_und:
        real_reg = RegisterName::r14_und;
        break;
    case RegisterName::pc:
        real_reg = RegisterName::r15;
        break;
    default:
        real_reg = reg;
        break;
    };

    return real_reg;
}

int Arm7Tdmi::RunNextInstruction(Bus& bus)
{
    int cycles;
    uint32_t current_pc = ReadRegisterDirect(RegisterName::r15);
    // Fetch next opcode
    if (mCurrentPsr.state == OperatingState::Arm)
    {
        // Fetch a 32 bit ARM instruction
        uint32_t opcode = bus.Read32(current_pc);

        fmt::println("Executing ARM instruction at pc {:#X}...", current_pc);

        // Execute
        cycles = ExecuteArmInstruction(bus, opcode);
    }
    else
    {
        // Fetch a 16 bit THUMB instruction
        uint16_t opcode = bus.Read16(current_pc);

        fmt::println("Executing THUMB instruction at pc {:#X}...", current_pc);

        // Execute
        cycles = ExecuteThumbInstruction(bus, opcode);
    }

    // TODO figure out how to increment PC after arm/thumb transition. This is probably good enough for now
    // Check PSR again because executing an instruction may have changed it
    // Fetch PC again too because the executed instruction may have been a jump
    current_pc = ReadRegisterDirect(RegisterName::r15);
    if (mCurrentPsr.state == OperatingState::Arm)
    {
        WriteRegisterDirect(RegisterName::r15, current_pc + 4);
    }
    else
    {
        WriteRegisterDirect(RegisterName::r15, current_pc + 2);
    }
    
    return cycles;
}

int Arm7Tdmi::ExecuteArmInstruction(Bus& bus, uint32_t opcode)
{
    // Branch on those three bits to kick off decoding
    switch ((opcode >> 25) & 0b111)
    {
    case 0b101:
        return OpArmBranch(bus, opcode);
    default:
        panic(fmt::format("Unknown ARM instruction {:#X}!", opcode));
    }
}

int Arm7Tdmi::ExecuteThumbInstruction(Bus& bus, uint16_t opcode)
{
    panic(fmt::format("Unknown THUMB instruction {:#X}!", opcode));
}

// ARM instructions
int Arm7Tdmi::OpArmBranch(Bus& bus, uint32_t opcode)
{
    bool should_link = GetBit(opcode, 24);
    int32_t offset = SignExtend<int32_t, 26>(static_cast<int32_t>((opcode & 0xFFFFFF) << 2));
    uint32_t old_pc = ReadRegisterDirect(RegisterName::r15);
    uint32_t new_pc = old_pc + offset;
    // Add 4 to account for the prefetch and the 4 that will be added after execution
    WriteRegisterDirect(RegisterName::r15, new_pc + 4);

    if (should_link)
    {
        // Add 4 to the old pc so we return to the next instruction
        WriteRegisterDirect(RegisterName::r14, old_pc + 4);
    }

    return 1;
}