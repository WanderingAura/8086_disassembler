#include <dis86_num_types.h>
#include <dis86_instruction.h>
#include <iostream>
#include <fstream>
#include <array>
#include <cassert>
#include <cmath>

#define MAX_FIELD_NUM 16
#define DIRECT_ADDRESS_IDX 8

bool Instruction::NeedSize(OperandType type) {
    return (type == OperandType::MEMORY &&
        (opType == OpType::PUSH || opType == OpType::POP));
}

Instruction::Instruction(OpType type, Operand op1, Operand op2) : opType(type), operands{op1, op2} {}

Instruction::Instruction() : opType{}, operands{} {}

void Instruction::Print(){
    assert(opType != OpType::NONE && opType < OpType::NUM_OPS);

    std::string operandStrs[2];
    for (u32 i = 0; i < 2; i++) {
        if (NeedSize(operands[i].operandType)) {
            // NOTE: for some reason nasm requires a size to be specified
            // on instructions like push and pop even though they can
            // only operate on words.
            operandStrs[i] = "word " + operands[i].GetStr();
        } else {
            operandStrs[i] = operands[i].GetStr();
        }
    }

    u32 numOps = (operands[0].operandType != OperandType::NONE)
        + (operands[1].operandType != OperandType::NONE);
    std::string sep = numOps == 2 ? ", " : "";
    
    assert(opStrs[(u8)opType] != "");
    std::string instStr = opStrs[(u8)opType] + " "
        + operandStrs[0] + sep + operandStrs[1];

    std::cout << instStr << std::endl;
    return;
}


Instruction::operator bool() const {
    return opType != OpType::NONE;
}

// used for comparing instructions for testing
bool Instruction::operator==(const Instruction& rhs) const{
    return opType == rhs.opType &&
           operands[0] == rhs.operands[0] &&
           operands[1] == rhs.operands[1];
}

const std::array<std::string, (u8)OpType::NUM_OPS> Instruction::opStrs = {{
    "", "add", "sub", "cmp", "mov", "adc", "sbb", "push", "pop", "xchg", "in", "out",
    "xlat", "lea", "lds", "les", "lahf", "sahf", "pushf", "popf", "or", "and", "xor",
}};
