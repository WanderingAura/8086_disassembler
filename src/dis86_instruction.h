#pragma once
#include <dis86_num_types.h>
#include <string>
#include <array>

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
    RegisterIdx regIdx;
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
    IMMEDIATE,
    MEMORY,
};

struct Operand {
    OperandType operandType;
    union {
        EffectiveAddressExp address;
        Register reg;
        Immediate immediate;
    };

    bool operator==(const Operand& rhs) const;
};

enum class OpType : u8 {
    NONE,
    ADD,
    SUB,
    CMP,
    MOV,
    NUM_OPS
};


class Instruction {
public:
    void Print();

    explicit operator bool() const;

    Instruction(OpType type, Operand op1, Operand op2);

    Instruction();

    bool operator==(const Instruction& rhs) const; 
    

private:
    OpType opType;
    Operand operands[2];

    static const std::string registers[8][2];

    static const std::array<std::string, (u8)OpType::NUM_OPS> opStrs;

    static const std::string addressExps[9];

    std::string GetMemoryOpStr(const Operand op);
    std::string GetOperandStr(const Operand op);
};