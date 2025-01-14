#pragma once
#include <dis86_num_types.h>
#include <iostream>

enum class RegisterIdx : u8 {
    AL_AX,
    CL_CX,
    DL_DX,
    BL_BX,
    AH_SP,
    CH_BP,
    DH_SI,
    BH_DI,
};

enum class SegmentRegIdx : u8 {
    ES,
    CS,
    SS,
    DS,
};

enum class AddressExpIdx : u8 {
    BX_SI,
    BX_DI,
    BP_SI,
    BP_DI,
    SI,
    DI,
    BP,
    BX,
    DIRECT,
};

struct Register {
    union {
        RegisterIdx regIdx;
        SegmentRegIdx sRegIdx;
    };
    b8 isWide;
};

struct EffectiveAddressExp {
    AddressExpIdx expIdx;
    i16 disp;
};

struct Immediate {
    union {
        u16 immU16;
        i16 immI16;
    };
    u8 isWide;
};

enum class OperandType: u8 {
    NONE,
    REGISTER,
    SEG_REG,
    IMMEDIATE,
    MEMORY,
};

class Operand {
public:
    OperandType operandType;
    union {
        EffectiveAddressExp address;
        Register reg;
        Immediate immediate;
    };

    bool operator==(const Operand& rhs) const;
    friend std::ostream& operator<<(std::ostream s, const Operand& op);
    std::string GetStr() const;

private:
    std::string GetMemoryOpStr() const;

    static const std::string registers[8][2];
    static const std::string segRegisters[4];
    static const std::string addressExps[9];
};
