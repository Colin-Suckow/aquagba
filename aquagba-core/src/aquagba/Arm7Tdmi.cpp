#include "aquagba/Arm7Tdmi.hpp"
#include "aquagba/bus.hpp"
#include "aquagba/util.hpp"
#include "fmt/format.h"
#include <bit>

using namespace aquagba;

Arm7Tdmi::Arm7Tdmi()
{
    Reset();
}

void Arm7Tdmi::Reset()
{
    // Write old PC to r14_svc
    GetRegisterDirect(RegisterName::r15) = GetRegisterDirect(RegisterName::r14_svc);

    // Write old PSR to spsr_svc
    GetRegisterDirect(RegisterName::spsr_svc) = GetRegisterDirect(RegisterName::cpsr);

    // Enter supervisor mode
    mCurrentPsr.mode = ProcessorMode::Supervisor;

    // Set irq and fiq disable bits
    mCurrentPsr.irq_disable = true;
    mCurrentPsr.fiq_disable = true;

    // Reset PC to 0
    GetRegisterDirect(RegisterName::r15) = 0;

    // Switch to ARM mode
    mCurrentPsr.state = OperatingState::Arm;
}

uint32_t& Arm7Tdmi::GetRegisterDirect(RegisterName reg)
{
    return mRegisters[static_cast<int>(MapRegister(reg))];
}

uint32_t& Arm7Tdmi::GetRegisterNumber(uint8_t reg_num)
{
    if (reg_num <= 7 || reg_num == 15)
    {
        return mRegisters[reg_num];
    }
    else if (reg_num <= 12)
    {
        //r8-r12 can be either normal or fiq
        if (mCurrentPsr.mode == ProcessorMode::Fiq)
        {
            return mRegisters[reg_num + 8];
        }
        else
        {
            return mRegisters[reg_num];
        }
    }
    else if (reg_num <= 14)
    {
        switch (mCurrentPsr.mode)
        {
        case ProcessorMode::System:
        case ProcessorMode::User:
            return mRegisters[reg_num];
        case ProcessorMode::Fiq:
            return mRegisters[reg_num + 8];
        case ProcessorMode::Supervisor:
            return mRegisters[reg_num + 10];
        case ProcessorMode::Abort:
            return mRegisters[reg_num + 12];
        case ProcessorMode::Irq:
            return mRegisters[reg_num + 14];
        case ProcessorMode::Undefined:
            return mRegisters[reg_num + 16];
        default:
            panic(fmt::format("Arm7Tdmi::GetRegisterNumber; Unknown processor mode! mode = {}", static_cast<uint32_t>(mCurrentPsr.mode)));
        }
    }
    else
    {
        panic(fmt::format("Arm7Tdmi::GetRegisterNumber; Unknown register number! num = {}", reg_num));
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
    uint32_t current_pc = GetRegisterDirect(RegisterName::r15);
    // Fetch next opcode
    if (mCurrentPsr.state == OperatingState::Arm)
    {
        // Fetch a 32 bit ARM instruction
        uint32_t opcode = bus.Read32(current_pc);

        // Check the cond field and execute if true
        if (ArmCheckCond((opcode >> 28) & 0b1111))
        {
            fmt::println("Executing ARM instruction at pc {:#X}...", current_pc);

            // Execute
            cycles = ExecuteArmInstruction(bus, opcode);
        }
        else
        {
            fmt::println("Skipping ARM instruction at {:#X} because cond was false...", current_pc);
        }
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
    current_pc = GetRegisterDirect(RegisterName::r15);
    if (mCurrentPsr.state == OperatingState::Arm)
    {
        GetRegisterDirect(RegisterName::r15) = current_pc + 4;
    }
    else
    {
        GetRegisterDirect(RegisterName::r15) = current_pc + 2;
    }
    
    return cycles;
}

int Arm7Tdmi::ExecuteArmInstruction(Bus& bus, uint32_t opcode)
{
    if (((opcode >> 25) & 0b111) == 0b101)
    {
        return OpArmBranch(bus, opcode);
    }
    else if (((opcode >> 26) & 0b11) == 0)
    {
        return OpArmDataProc(bus, opcode);
    }
    else if (((opcode >> 26) & 0b11) == 0b01)
    {
        return OpArmSingleDataTransfer(bus, opcode);
    }


    // If we get down here then decoding was unable to find an instruction
    panic(fmt::format("Unknown ARM instruction {0:#X} ({0:#034b})!", opcode));
}

int Arm7Tdmi::ExecuteThumbInstruction(Bus& bus, uint16_t opcode)
{
    panic(fmt::format("Unknown THUMB instruction {:#X}!", opcode));
}

bool Arm7Tdmi::ArmCheckCond(uint8_t cond)
{
    switch (cond)
    {
    case 0b0000:
        return mCurrentPsr.zero;
    case 0b0001:
        return !mCurrentPsr.zero;
    case 0b0010:
        return mCurrentPsr.carry;
    case 0b0011:
        return !mCurrentPsr.carry;
    case 0b0100:
        return mCurrentPsr.negative;
    case 0b0101:
        return !mCurrentPsr.negative;
    case 0b0110:
        return mCurrentPsr.overflow;
    case 0b0111:
        return !mCurrentPsr.overflow;
    case 0b1000:
        return mCurrentPsr.carry && !mCurrentPsr.zero;
    case 0b1001:
        return !mCurrentPsr.carry || mCurrentPsr.zero;
    case 0b1010:
        return mCurrentPsr.negative == mCurrentPsr.overflow;
    case 0b1011:
        return mCurrentPsr.negative != mCurrentPsr.overflow;
    case 0b1100:
        return !mCurrentPsr.zero && (mCurrentPsr.negative == mCurrentPsr.overflow);
    case 0b1101:
        return mCurrentPsr.zero || (mCurrentPsr.negative != mCurrentPsr.overflow);
    case 0b1110:
        return true;
    default:
        panic(fmt::format("Unknown ARM cond field value = {:#X}", cond));
    }
}

uint32_t Arm7Tdmi::FetchDataProcessingOper2(const uint32_t opcode, const bool set_carry)
{
    bool operand_is_immediate = GetBit(opcode, 25);
    uint32_t oper2;

    if (operand_is_immediate)
    {
        uint32_t imm = opcode & 0xFF;
        
        // Decode and execute shift on imm
        uint32_t rotate = ((opcode >> 8) & 0xF) * 2; // Rotate is doubled because the manual said so. See 4.5.3

        oper2 = std::rotr(imm, rotate);
    }
    else
    {
        // Register op2
        uint8_t reg_num = opcode & 0xF;
        uint32_t shift = (opcode >> 4) & 0xFF;
        ArmShiftType type = static_cast<ArmShiftType>((shift >> 1) & 0b11);
        uint8_t shift_amount;
        bool register_shift = GetBit(shift, 0);
        
        if (register_shift)
        {
            // Register shift
            uint8_t shift_reg_num = (shift >> 8) & 0xF;
            shift_amount = static_cast<uint8_t>(GetRegisterNumber(shift_reg_num) & 0xFF);
        }
        else
        {
            // Immediate shift
            shift_amount = static_cast<uint8_t>((shift >> 5) & 0b11111);
        }

        uint32_t shifted = GetRegisterNumber(reg_num);

        switch (type)
        {
        case ArmShiftType::ArithRight:
            shifted = static_cast<int32_t>(shifted) >> shift_amount;
            break;
        case ArmShiftType::LogicalRight:
            shifted = shifted >> shift_amount;
            break;
        case ArmShiftType::LogicalLeft:
            shifted = shift << shift_amount;
            break;
        case ArmShiftType::RotRight:
            shifted = std::rotr(shift, shift_amount);
            break;
        default:
            panic(fmt::format("Unknown shift type! type = {}", static_cast<uint32_t>(type)));
        };

        oper2 = shifted;
    }
    return oper2;
}

// ARM instructions
int Arm7Tdmi::OpArmBranch(Bus& bus, uint32_t opcode)
{
    bool should_link = GetBit(opcode, 24);
    int32_t offset = SignExtend<int32_t, 26>(static_cast<int32_t>((opcode & 0xFFFFFF) << 2));
    uint32_t old_pc = GetRegisterDirect(RegisterName::r15);
    uint32_t new_pc = old_pc + offset;
    // Add 4 to account for the prefetch and the 4 that will be added after execution
    GetRegisterDirect(RegisterName::r15) = new_pc + 4;

    if (should_link)
    {
        // Add 4 to the old pc so we return to the next instruction
        GetRegisterDirect(RegisterName::r14) = old_pc + 4;
    }

    return 1;
}

int Arm7Tdmi::OpArmDataProc(Bus& bus, uint32_t opcode)
{
    bool set_condition_codes = GetBit(opcode, 20);
    uint8_t data_proc_opcode = ((opcode >> 21) & 0b1111);

    uint32_t oper1 = GetRegisterNumber((opcode >> 16) & 0xF);
    uint32_t oper2 = FetchDataProcessingOper2(opcode, true);
    uint32_t& result_reg = GetRegisterNumber((opcode >> 12) & 0xF);
    uint32_t result;

    switch (data_proc_opcode)
    {
    case 0b1010:
    {
        result = oper1 - oper2;
        fmt::println("Executing CMP. {} - {} = {}", oper1, oper2, result);
        break;
    }
    case 0b1101:
    {
        result = oper2;
        result_reg = oper2;
        fmt::println("Executing MOV. {:#X} -> reg({})", oper2, (opcode >> 12) & 0xF);
        break;
    }
    case 0b1001:
        result = oper1 ^ oper2;
        fmt::println("Executing TEQ. {} ^ {} = {}", oper1, oper2, result);
        break;
    default:
        panic(fmt::format("Unknown ARM data processing instruction! Opcode = {:#b}", data_proc_opcode));
    }
    
    if (set_condition_codes)
    {
        // Carry is set by FetchDataProcessingOper2 because this is a logical instruction
        mCurrentPsr.zero = result == 0;
        mCurrentPsr.negative = GetBit(result, 31);
        mCurrentPsr.overflow = (GetBit(oper1, 31) != GetBit(oper2, 31)) && GetBit(oper2, 31) == GetBit(result, 31);
    }

    return 1;
}

int Arm7Tdmi::OpArmSingleDataTransfer(Bus& bus, uint32_t opcode)
{
    uint32_t offset;
    if (GetBit(opcode, 25))
    {
        // Register offset
        uint8_t reg_num = opcode & 0xF;
        uint32_t shift = (opcode >> 4) & 0xFF;
        ArmShiftType type = static_cast<ArmShiftType>((shift >> 1) & 0b11);
        uint8_t shift_reg_num = (shift >> 8) & 0xF;
        uint8_t shift_amount = static_cast<uint8_t>(GetRegisterNumber(shift_reg_num) & 0xFF);
        uint32_t shifted = GetRegisterNumber(reg_num);

        switch (type)
        {
        case ArmShiftType::ArithRight:
            shifted = static_cast<int32_t>(shifted) >> shift_amount;
            break;
        case ArmShiftType::LogicalRight:
            shifted = shifted >> shift_amount;
            break;
        case ArmShiftType::LogicalLeft:
            shifted = shift << shift_amount;
            break;
        case ArmShiftType::RotRight:
            shifted = std::rotr(shift, shift_amount);
            break;
        default:
            panic(fmt::format("Unknown shift type! type = {}", static_cast<uint32_t>(type)));
        };

        offset = shifted;
    }
    else
    {
        // Immediate offste
        offset = opcode & 0xFFF;
    }

    bool pre_bit = GetBit(opcode, 24);
    bool up_bit = GetBit(opcode, 23);
    bool byte_bit = GetBit(opcode, 22);
    bool write_back_bit = GetBit(opcode, 21);
    bool load_bit = GetBit(opcode, 20);

    uint32_t& base_reg = GetRegisterNumber((opcode >> 16) & 0xF);
    uint32_t& source_dest_reg = GetRegisterNumber((opcode >> 12) & 0xF);
    uint32_t addr = base_reg;

    if (pre_bit)
    {
        // Pre increment enable
        if (up_bit)
        {
            // Add offset
            addr += offset;
        }
        else
        {
            // Subtract offset
            addr -= offset;
        }
    }

    if (load_bit)
    {
        if (byte_bit)
        {
            // load byte from memory
            // TODO Handle unaligned reads
            if (addr % 4 != 0) panic(fmt::format("Unaligned read! Addr {} pc {}", addr, GetRegisterDirect(RegisterName::r15)));
            source_dest_reg = bus.Read32(addr);
        }
        else
        {
            // load word from memory
            source_dest_reg = bus.Read8(addr);
        }
    }
    else
    {
        if (byte_bit)
        {
            // store byte to memeory
            panic("Tried to write word, not implemented");
        }
        else
        {
            // store word to memory
            // TODO Handle unaligned writes
            if (addr % 4 != 0) panic(fmt::format("Unaligned write! Addr {} pc {}", addr, GetRegisterDirect(RegisterName::r15)));
            panic("Tried to write byte, not implemented");
        }
    }

    if (!pre_bit)
    {
        // Post increment
        if (up_bit)
        {
            // Add offset
            addr += offset;
        }
        else
        {
            // Subtract offset
            addr -= offset;
        }
    }

    // Write back if write back bit is set OR if post increment is enabled
    if (write_back_bit || !pre_bit)
    {
        base_reg = addr;
    }

    return 1;
}