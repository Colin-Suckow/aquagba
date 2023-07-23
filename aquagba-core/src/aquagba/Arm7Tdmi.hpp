#pragma once

#include <array>
#include <stdint.h>
#include <unordered_map>
#include "aquagba/bus.hpp"

namespace aquagba
{

enum class RegisterName
{
    r1,
    r2,
    r3,
    r4,
    r5,
    r6,
    r7,
    r8,
    r9,
    r10,
    r11,
    r12,
    r13,
    r14,
    r15,

    r8_fiq,
    r9_fiq,
    r10_fiq,
    r11_fiq,
    r12_fiq,
    r13_fiq,
    r14_fiq,

    r13_svc,
    r14_svc,

    r13_abt,
    r14_abt,

    r13_irq,
    r14_irq,

    r13_und,
    r14_und,

    cpsr,
    spsr_fiq,
    spsr_svc,
    spsr_abt,
    spsr_irq,
    spsr_und,

    //Thumb registers
    sp,
    sp_fiq,
    sp_svc,
    sp_abt,
    sp_irq,
    sp_und,

    lr,
    lr_fiq,
    lr_svc,
    lr_abt,
    lr_irq,
    lr_und,

    pc
};

enum class OperatingState
{
    Arm = 0,
    Thumb = 1
};

enum class ProcessorMode
{
    User = 0b10000,
    Fiq = 0b10001,
    Irq = 0b10010,
    Supervisor = 0b10011,
    Abort = 0b10111,
    Undefined = 0b11011,
    System = 0b11111
};

enum class ArmShiftType
{
    LogicalLeft = 0b00,
    LogicalRight = 0b01,
    ArithRight = 0b10,
    RotRight = 0b11
};

struct ProgramStatus
{
    ProcessorMode mode;
    OperatingState state;
    bool fiq_disable;
    bool irq_disable;
    bool overflow;
    bool carry; // Carry, borrow, or extend
    bool zero;
    bool negative;

    uint32_t AsBinary()
    {
        uint32_t result;

        result |= static_cast<uint32_t>(mode);
        result |= static_cast<uint32_t>(state) << 5;
        result |= static_cast<uint32_t>(fiq_disable) << 6;
        result |= static_cast<uint32_t>(irq_disable) << 7;
        result |= static_cast<uint32_t>(overflow) << 28;
        result |= static_cast<uint32_t>(carry) << 29;
        result |= static_cast<uint32_t>(zero) << 30;
        result |= static_cast<uint32_t>(negative) << 31;

        return result;
    }

    static ProgramStatus FromBinary(uint32_t val)
    {
        ProgramStatus result;
        result.mode = static_cast<ProcessorMode>(val & 0b11111);
        result.state = static_cast<OperatingState>((val >> 5) & 0x1);
        result.fiq_disable = static_cast<bool>((val >> 6) & 0x1);
        result.irq_disable = static_cast<bool>((val >> 7) & 0x1);
        result.overflow = static_cast<bool>((val >> 28) & 0x1);
        result.carry = static_cast<bool>((val >> 29) & 0x1);
        result.zero = static_cast<bool>((val >> 30) & 0x1);
        result.negative = static_cast<bool>((val >> 31) & 0x1);

        return result;
    }
};



class Arm7Tdmi
{
public:
    Arm7Tdmi();

    void Reset();
    uint32_t& GetRegisterDirect(RegisterName reg);
    uint32_t& GetRegisterNumber(uint8_t reg_num);

    // Fetch and run the next instruction. Returns the number of cycles it took
    int RunNextInstruction(Bus& bus);

private:
    // Instructions
    int OpArmBranch(Bus& bus, uint32_t opcode);
    int OpArmDataProc(Bus& bus, uint32_t opcode);
    int OpArmSingleDataTransfer(Bus& bus, uint32_t opcode);

    // Utils
    // Checks cond and returns true if the instruction should be executing. False if should skip
    bool ArmCheckCond(uint8_t cond);

    uint32_t FetchDataProcessingOper2(const uint32_t opcode, const bool set_carry);
    
    int ExecuteArmInstruction(Bus& bus, uint32_t opcode);
    int ExecuteThumbInstruction(Bus& bus, uint16_t opcode);

    // Map thumb register to the real register
    RegisterName MapRegister(RegisterName reg);

    std::array<uint32_t, 37> mRegisters;
    std::unordered_map<ProcessorMode, ProgramStatus> mSavedPsrs;
    ProgramStatus mCurrentPsr;
};

}