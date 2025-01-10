#pragma once
#include <dis86_num_types.h>
#include <string>
#include <array>

struct Register {
    u8 regIdx;
    u8 isWide;
};

struct EffectiveAddressExp {
    u8 regIdx;
    u16 disp;
};

enum class OperandType: u8 {
    NONE,
    REGISTER,
    IMMEDIATE,
    MEMORY,
    MEM_DISP,
};

struct Operand {
    OperandType operandType;
    union {
        EffectiveAddressExp address;
        Register reg;
        u16 immU16;
    };
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

    explicit operator bool() const {
        return this->opType != OpType::NONE;
    }

    Operand operands[2];
private:
    OpType opType;

    static const std::string registers[8][2];

    static const std::array<std::string, (u8)OpType::NUM_OPS> opStrs;

    static const std::string addressExps[9];

    std::string GetMemoryOpStr(const Operand op);
    std::string GetOperandStr(const Operand op);
};