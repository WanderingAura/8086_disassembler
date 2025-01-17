#pragma once

#include <dis86_num_types.h>
#include <dis86_instruction.h>

#include <array>
#include <fstream>

#define MAX_FIELD_NUM 16

enum BitsUsage : u8{
    Opcode,
    Reg,
    SR,
    Mod,
    RegMem,
    Direction,
    Width,
    SignExt,
    Disp,
    Data,

    HasDisp,
    HasData,
    WDataIfW,
    RMIsW,
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
    InstStream(std::istream *binFile);
private:
     // if too big may need to malloc memory to prevent stack overflow
    u8 bytes[1024*256];
    std::streamsize size;
    u32 currentInstPointer;
    u32 readPointer;

    static const InstructionFormat formats[58];

    static inline Operand GetRegOperand(u8 regVal, u8 widthVal);
    static inline Operand GetSegRegOperand(u8 regVal);

    u8 NextByte();
    u16 ParseData(bool isWide, bool isSignExt);

    void GetBitFields(u32 &bitFieldFlags,
        std::array<u32, BitsUsage::NumElements>& bitFieldValues,
        const std::array<BitField, MAX_FIELD_NUM>& fields);

    Instruction TryDecode(InstructionFormat format);
};
