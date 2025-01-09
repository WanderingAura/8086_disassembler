#pragma once

#include <dis86_num_types.h>
#include <dis86_instruction.h>

#include <array>
#include <fstream>

#define MAX_FIELD_NUM 16

enum BitsUsage : u8{
    Opcode,
    Reg,
    Mod,
    RegMem,
    Direction,
    Width,
    Disp,
    Data,

    HasDisp,
    HasData,
    WDataIfW,
    NumElements,
};

struct BitField {
    BitsUsage name = Opcode;
    u8 numBits = 0;
    u8 val = 0;
};

struct InstructionFormat {
    OpType op;
    std::array<BitField, 16> fields;
};

class InstStream {
public:
    Instruction NextInstruction();
    InstStream(std::ifstream *binFile);
private:
    u8 bytes[1024*1024];
    std::streamsize size;
    u32 currentInstPointer;
    u32 readPointer;

    static const InstructionFormat formats[2];

    static inline Operand GetRegOperand(u8 regVal, u8 widthVal);

    u8 NextByte();
    u16 ParseData(bool isWide);

    void GetBitFields(u32 &bitFieldFlags, u32 *bitFieldValues, std::array<BitField, MAX_FIELD_NUM> fields);

    Instruction TryDecode(InstructionFormat format);
};
