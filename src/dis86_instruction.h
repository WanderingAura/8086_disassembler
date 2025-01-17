#pragma once
#include <dis86_num_types.h>
#include <string>
#include <array>
#include <dis86_operand.h>

enum class OpType : u8 {
    NONE,
    ADD,
    SUB,
    CMP,
    MOV,
    ADC,
    SBB,
    PUSH,
    POP,
    XCHG,
    IN,
    OUT,
    XLAT,
    LEA,
    LDS,
    LES,
    LAHF,
    SAHF,
    PUSHF,
    POPF,
    OR,
    AND,
    XOR,
    INC,
    AAA,
    DAA,
    DEC,
    NEG,
    AAS,
    DAS,
    MUL,
    IMUL,
    AAM,
    DIV,
    IDIV,
    AAD,
    CBW,
    CWD,
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

    static const std::array<std::string, (u8)OpType::NUM_OPS> opStrs;

    bool NeedSize(OperandType type);
};