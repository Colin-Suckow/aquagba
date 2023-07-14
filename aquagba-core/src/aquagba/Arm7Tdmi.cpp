#include "aquagba/Arm7Tdmi.hpp"

aquagba::Arm7Tdmi::Arm7Tdmi()
{
    Reset();
}

void aquagba::Arm7Tdmi::Reset()
{
    // Write old PC to r14_svc
    WriteRegisterDirect(aquagba::RegisterName::r15, ReadRegisterDirect(aquagba::RegisterName::r14_svc));

    // Write old PSR to spsr_svc
    WriteRegisterDirect(aquagba::RegisterName::spsr_svc, ReadRegisterDirect(aquagba::RegisterName::cpsr));

    // Enter supervisor mode
    mCurrentMode = aquagba::ProcessorMode::Supervisor;

    // Set irq and fiq disable bits
    mCurrentPsr.irq_disable = true;
    mCurrentPsr.fiq_disable = true;

    // Reset PC to 0
    WriteRegisterDirect(aquagba::RegisterName::r15, 0);

    // Switch to ARM mode
    mCurrentPsr.state = aquagba::OperatingState::Arm;
}

void aquagba::Arm7Tdmi::WriteRegisterDirect(RegisterName reg, uint32_t value)
{
    switch (reg)
    {
    case aquagba::RegisterName::cpsr:
        mCurrentPsr = aquagba::ProgramStatus::FromBinary(value);
        break;
    case aquagba::RegisterName::spsr_fiq:
        mSavedPsrs[aquagba::ProcessorMode::Fiq] = aquagba::ProgramStatus::FromBinary(value);
        break;
    case aquagba::RegisterName::spsr_svc:
        mSavedPsrs[aquagba::ProcessorMode::Supervisor] = aquagba::ProgramStatus::FromBinary(value);
        break;
    case aquagba::RegisterName::spsr_abt:
        mSavedPsrs[aquagba::ProcessorMode::Abort] = aquagba::ProgramStatus::FromBinary(value);
        break;
    case aquagba::RegisterName::spsr_irq:
        mSavedPsrs[aquagba::ProcessorMode::Irq] = aquagba::ProgramStatus::FromBinary(value);
        break;
    case aquagba::RegisterName::spsr_und:
        mSavedPsrs[aquagba::ProcessorMode::Undefined] = aquagba::ProgramStatus::FromBinary(value);
        break;
    default:
         mRegisters[static_cast<int>(MapRegister(reg))] = value;
         break;
    }
}

uint32_t aquagba::Arm7Tdmi::ReadRegisterDirect(RegisterName reg)
{
    switch (reg)
    {
    case aquagba::RegisterName::cpsr:
        return mCurrentPsr.AsBinary();
    case aquagba::RegisterName::spsr_fiq:
        return mSavedPsrs[aquagba::ProcessorMode::Fiq].AsBinary();
    case aquagba::RegisterName::spsr_svc:
        return mSavedPsrs[aquagba::ProcessorMode::Supervisor].AsBinary();
    case aquagba::RegisterName::spsr_abt:
        return mSavedPsrs[aquagba::ProcessorMode::Abort].AsBinary();
    case aquagba::RegisterName::spsr_irq:
        return mSavedPsrs[aquagba::ProcessorMode::Irq].AsBinary();
    case aquagba::RegisterName::spsr_und:
        return mSavedPsrs[aquagba::ProcessorMode::Undefined].AsBinary();
    default:
         return mRegisters[static_cast<int>(MapRegister(reg))];
    }
}

aquagba::RegisterName aquagba::Arm7Tdmi::MapRegister(aquagba::RegisterName reg)
{
    aquagba::RegisterName real_reg;

    // Map thumb registers to real registers
    switch (reg)
    {
    case aquagba::RegisterName::sp:
        real_reg = aquagba::RegisterName::r13;
        break;
    case aquagba::RegisterName::sp_fiq:
        real_reg = aquagba::RegisterName::r13_fiq;
        break;
    case aquagba::RegisterName::sp_svc:
        real_reg = aquagba::RegisterName::r13_svc;
        break;
    case aquagba::RegisterName::sp_abt:
        real_reg = aquagba::RegisterName::r13_abt;
        break;
    case aquagba::RegisterName::sp_irq:
        real_reg = aquagba::RegisterName::r13_irq;
        break;
    case aquagba::RegisterName::sp_und:
        real_reg = aquagba::RegisterName::r13_und;
        break;
    case aquagba::RegisterName::lr:
        real_reg = aquagba::RegisterName::r14;
        break;
    case aquagba::RegisterName::lr_fiq:
        real_reg = aquagba::RegisterName::r14_fiq;
        break;
    case aquagba::RegisterName::lr_svc:
        real_reg = aquagba::RegisterName::r14_svc;
        break;
    case aquagba::RegisterName::lr_abt:
        real_reg = aquagba::RegisterName::r14_abt;
        break;
    case aquagba::RegisterName::lr_irq:
        real_reg = aquagba::RegisterName::r14_irq;
        break;
    case aquagba::RegisterName::lr_und:
        real_reg = aquagba::RegisterName::r14_und;
        break;
    case aquagba::RegisterName::pc:
        real_reg = aquagba::RegisterName::r15;
        break;
    default:
        real_reg = reg;
        break;
    };

    return real_reg;
}