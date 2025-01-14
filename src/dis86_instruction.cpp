#include <dis86_num_types.h>
#include <dis86_instruction.h>
#include <iostream>
#include <fstream>
#include <array>
#include <cassert>
#include <cmath>

#define MAX_FIELD_NUM 16
#define DIRECT_ADDRESS_IDX 8


std::string Instruction::GetMemoryOpStr(const Operand& op) {
    u8 expIdx = (u8)op.address.expIdx;
    i16 disp = op.address.disp;
    assert(expIdx < 9 && disp <= 0xffff);
    std::string dispStr = std::to_string(std::abs(disp));
    if (expIdx == 8) {
        return disp < 0 ? "[-" + dispStr + "]" : "[" + dispStr + "]";
    }
    std::string regExp = addressExps[expIdx];
    if (disp == 0) {
        return "[" + regExp + "]";
    }
    std::string signStr = " + ";
    if (disp < 0) {
        signStr[1] = '-';
    }
    return "[" + regExp + signStr + dispStr + "]";
}

Instruction::Instruction(OpType type, Operand op1, Operand op2) : opType(type), operands{op1, op2} {}

Instruction::Instruction() : opType{}, operands{} {}

std::string Instruction::GetOperandStr(const Operand& op) {
    switch (op.operandType) {
        case OperandType::NONE: {
            return "";
        }
        case OperandType::REGISTER: {
            u8 regIdx = (u8)op.reg.regIdx;
            u8 isWide = op.reg.isWide;
            assert(regIdx < 8 && isWide < 2);
            return registers[regIdx][isWide];
        }
        case OperandType::SEG_REG: {
            u8 sRegIdx = (u8)op.reg.sRegIdx;
            assert(sRegIdx < 4);
            return segRegisters[sRegIdx];
        }
        case OperandType::IMMEDIATE: {
            // if the immediate is a byte then truncate it first
            i16 imm = op.immediate.immI16;
            if (!op.immediate.isWide) {
                imm = (i8)imm;
            }
            return (op.immediate.isWide ? "word " : "byte ")  + std::to_string(op.immediate.immI16);
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
    if (operands[1].operandType != OperandType::NONE &&
        operands[0].operandType != OperandType::NONE) {
        sep = ", ";
    }
    assert(opStrs[(u8)opType] != "");
    std::string instStr = opStrs[(u8)opType] + " " + op1Str + sep + op2Str;

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
bool Operand::operator==(const Operand& rhs) const {
    if (operandType != rhs.operandType) {
        return false;
    }

    switch (operandType) {
        case OperandType::MEMORY:
            return address.expIdx == rhs.address.expIdx &&
                   address.disp == rhs.address.disp;
        case OperandType::SEG_REG:
        case OperandType::REGISTER:
            return reg.regIdx == rhs.reg.regIdx &&
                   reg.isWide == rhs.reg.isWide;
        case OperandType::IMMEDIATE:
            if (immediate.isWide != rhs.immediate.isWide) {
                return false;
            }
            if (immediate.isWide) {
                return immediate.immI16 == rhs.immediate.immI16;
            } else {
                return (i8)immediate.immI16 == (i8)rhs.immediate.immI16;
            }
        default:
            return false;
    }
}

const std::array<std::string, (u8)OpType::NUM_OPS> Instruction::opStrs = {{
    "", "add", "sub", "cmp", "mov", "adc", "sbb", "push", "pop", "xchg", "in", "out",
    "xlat", "lea", "lds", "les", "lahf", "sahf", "pushf", "popf", "or", "and", "xor",
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

const std::string Instruction::segRegisters[] = {
    "es", "cs", "ss", "ds",
};