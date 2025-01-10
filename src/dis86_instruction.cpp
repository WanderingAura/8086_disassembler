#include <dis86_num_types.h>
#include <dis86_instruction.h>
#include <iostream>
#include <fstream>
#include <array>
#include <cassert>

#define MAX_FIELD_NUM 16
#define DIRECT_ADDRESS_IDX 8


std::string Instruction::GetMemoryOpStr(const Operand op) {
    u8 expIdx = op.address.regIdx;
    u16 disp = op.address.disp;
    assert(expIdx < 9 && disp <= 0xffff);
    std::string dispStr = std::to_string(disp);
    if (expIdx == 8) {
        return "[" + dispStr + "]";
    }
    std::string regExp = addressExps[expIdx];
    if (disp == 0) {
        return "[" + regExp + "]";
    }
    return "[" + regExp + " " + dispStr + "]";
}

std::string Instruction::GetOperandStr(const Operand op) {
    switch (op.operandType) {
        case OperandType::NONE: {
            return "";
        }
        case OperandType::REGISTER: {
            u8 regIdx = op.reg.regIdx;
            u8 isWide = op.reg.isWide;
            assert(regIdx < 8 && isWide < 2);
            return registers[regIdx][isWide];
        }
        case OperandType::IMMEDIATE: {
            return std::to_string(op.immU16);
        }
        case OperandType::MEMORY: {
            return GetMemoryOpStr(op);
        }
        default: {
            std::cerr << "unsupposed operand type found" << std::to_string((u8)op.operandType) << std::endl;
            return "";
        }
    }
}

void Instruction::Print(){
    assert(opType != OpType::NONE && opType < OpType::NUM_OPS);

    std::string op1Str = GetOperandStr(operands[0]);
    std::string sep = "";
    std::string op2Str = GetOperandStr(operands[1]);
    if (operands[1].operandType != OperandType::NONE) {
        sep = ", ";
    }
    std::string instStr = opStrs[(u8)opType] + " " + op1Str + sep + op2Str;

    std::cout << instStr << std::endl;
    return;
}

const std::array<std::string, (u8)OpType::NUM_OPS> Instruction::opStrs = {{
    "", "add", "sub", "cmp", "mov"
}};

const std::string Instruction::registers[8][2] = {
    {"al", "ax"}, // 000
    {"cl", "cx"}, // 001
    {"dl", "dx"}, // 010
    {"bl", "bx"}, // 011
    {"ah", "sp"}, // 100
    {"ch", "bp"}, // 101
    {"dh", "si"}, // 110
    {"bh", "di"}, // 111
};

const std::string Instruction::addressExps[9] = {
    "bx + si",
    "bx + di",
    "bp + si",
    "bp + di",
    "si",
    "di",
    "bp",
    "bx",
    "", // direct address (no expression)
};