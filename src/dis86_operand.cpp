#include <dis86_operand.h>
#include <cassert>
#include <string>
#include <array>

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

std::string Operand::GetMemoryOpStr() const {
    u8 expIdx = (u8)address.expIdx;
    i16 disp = address.disp;
    assert(expIdx < 9 && disp <= 0xffff);
    std::string dispStr = std::to_string(std::abs(disp));
    std::string regExp = addressExps[expIdx];
    std::string memoryExp;
    if (expIdx == (u8)AddressExpIdx::DIRECT) {
        memoryExp = disp < 0 ? "[-" + dispStr + "]" : "[" + dispStr + "]";
    } else if (disp == 0) {
        memoryExp =  "[" + regExp + "]";
    } else {
        std::string signStr = " + ";
        if (disp < 0) {
            signStr[1] = '-';
        }
        memoryExp =  "[" + regExp + signStr + dispStr + "]";
    }

    return memoryExp;
}

std::string Operand::GetStr() const {
    switch (operandType) {
        case OperandType::NONE: {
            return "";
        }
        case OperandType::REGISTER: {
            u8 regIdx = (u8)reg.regIdx;
            u8 isWide = reg.isWide;
            assert(regIdx < 8 && isWide < 2);
            return registers[regIdx][isWide];
        }
        case OperandType::SEG_REG: {
            u8 sRegIdx = (u8)reg.sRegIdx;
            assert(sRegIdx < 4);
            return segRegisters[sRegIdx];
        }
        case OperandType::IMMEDIATE: {
            // if the immediate is a byte then truncate it first
            i16 imm = immediate.immI16;
            if (!immediate.isWide) {
                imm = (i8)imm;
            }
            // TODO: currently word/byte appears before immediates no matter what
            // this causes a bug for shift instructions. when the output of the
            // disassembler gets fed back into nasm, nasm will read 'byte'/'word' and
            // assume the encoding should be shr rm16, imm8 rather than shr rm16, 1.
            // goal: refactor so that byte/word appear only when needed
            return (immediate.isWide ? "word " : "byte ")  + std::to_string(immediate.immI16);
        }
        case OperandType::MEMORY: {
            return this->GetMemoryOpStr();
        }
        default: {
            std::cerr << "unsupposed operand type found" << std::to_string((u8)operandType) << std::endl;
            return "";
        }
    }
}

std::ostream& operator<<(std::ostream s, const Operand& op) {
    return s << op.GetStr();
}

const std::string Operand::registers[8][2] = {
    {"al", "ax"}, // 000
    {"cl", "cx"}, // 001
    {"dl", "dx"}, // 010
    {"bl", "bx"}, // 011
    {"ah", "sp"}, // 100
    {"ch", "bp"}, // 101
    {"dh", "si"}, // 110
    {"bh", "di"}, // 111
};

const std::string Operand::addressExps[9] = {
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

const std::string Operand::segRegisters[] = {
    "es", "cs", "ss", "ds",
};