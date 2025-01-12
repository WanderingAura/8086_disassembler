#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <dis86_instruction_stream.h>

static inline Operand GetAddressOperand(AddressExpIdx expIdx, i16 disp) {
    Operand res = {};
    res.operandType = OperandType::MEMORY;
    res.address.expIdx = expIdx;
    res.address.disp = disp;
    return res;
}

static inline Operand GetRegOperand(RegisterIdx idx, b8 isWide) {
    Operand res = {};
    res.operandType = OperandType::REGISTER;
    res.reg.regIdx = idx;
    res.reg.isWide = isWide;
    return res;
}

class ByteStreamBuf : public std::streambuf {
public:
    ByteStreamBuf(const char* data, std::size_t size) {
        char* start = const_cast<char*>(data);
        setg(start, start, start + size);
    }
};

TEST(MOV_TEST, RM2Reg_Decoding) {
    // TODO: execute nasm on rm2reg.asm and confirm the bytes are the same
    // see rm2reg.asm

    const Instruction expected = Instruction(
        OpType::MOV,
        GetRegOperand(RegisterIdx::AL_AX, true),
        GetRegOperand(RegisterIdx::BL_BX, true)
    );

    const Instruction expectedInsts[] = {
        // reg to reg mov instructions
        Instruction(OpType::MOV, 
            GetRegOperand(RegisterIdx::AL_AX, true),
            GetRegOperand(RegisterIdx::BL_BX, true)),
        Instruction(OpType::MOV, 
            GetRegOperand(RegisterIdx::BL_BX, true),
            GetRegOperand(RegisterIdx::CH_BP, true)),
        Instruction(OpType::MOV, 
            GetRegOperand(RegisterIdx::DH_SI, true),
            GetRegOperand(RegisterIdx::BH_DI, true)),
        Instruction(OpType::MOV, 
            GetRegOperand(RegisterIdx::BL_BX, false),
            GetRegOperand(RegisterIdx::AH_SP, false)),
        Instruction(OpType::MOV, 
            GetRegOperand(RegisterIdx::AH_SP, false),
            GetRegOperand(RegisterIdx::AL_AX, false)),
        Instruction(OpType::MOV, 
            GetRegOperand(RegisterIdx::CL_CX, false),
            GetRegOperand(RegisterIdx::DL_DX, false)),

        // address without displacement to/from reg
        Instruction(OpType::MOV, 
            GetRegOperand(RegisterIdx::AL_AX, true),
            GetAddressOperand(AddressExpIdx::BX_SI, 0)),
        Instruction(OpType::MOV, 
            GetRegOperand(RegisterIdx::BL_BX, true),
            GetAddressOperand(AddressExpIdx::BX_DI, 0)),
        Instruction(OpType::MOV, 
            GetAddressOperand(AddressExpIdx::BP_SI, 0),
            GetRegOperand(RegisterIdx::CL_CX, true)),
        Instruction(OpType::MOV, 
            GetAddressOperand(AddressExpIdx::BP_DI, 0),
            GetRegOperand(RegisterIdx::DL_DX, true)),
        Instruction(OpType::MOV, 
            GetAddressOperand(AddressExpIdx::DI, 0),
            GetRegOperand(RegisterIdx::BH_DI, true)),
        
        // address with displacement to/from reg
        Instruction(OpType::MOV, 
            GetRegOperand(RegisterIdx::AL_AX, true),
            GetAddressOperand(AddressExpIdx::BX_SI, 100)),
        Instruction(OpType::MOV, 
            GetAddressOperand(AddressExpIdx::BX_DI, -10),
            GetRegOperand(RegisterIdx::BL_BX, true)),
        Instruction(OpType::MOV, 
            GetRegOperand(RegisterIdx::BL_BX, true),
            GetAddressOperand(AddressExpIdx::BX_DI, 16000)),
        Instruction(OpType::MOV, 
            GetAddressOperand(AddressExpIdx::BX, -18934),
            GetRegOperand(RegisterIdx::AL_AX, true)),
        
        // direct address
        Instruction(OpType::MOV, 
            GetAddressOperand(AddressExpIdx::DIRECT, 13495),
            GetRegOperand(RegisterIdx::CH_BP, true)),
        Instruction(OpType::MOV, 
            GetRegOperand(RegisterIdx::CH_BP, true),
            GetAddressOperand(AddressExpIdx::DIRECT, 39)),

    };

    const char rm2reg[] = {
        0x89,0xd8,0x89,0xeb,0x89,0xfe,0x88,0xe3,
        0x88,0xc4,0x88,0xd1,0x8b,0x00,0x8b,0x19,
        0x89,0x0a,0x89,0x13,0x89,0x3d,0x8b,0x40,
        0x64,0x89,0x59,0xf6,0x8b,0x99,0x80,0x3e,
        0x89,0x87,0x0a,0xb6,0x89,0x2e,0xb7,0x34,
        0x8b,0x2e,0x27,0x00,
    };

    std::size_t size = sizeof(rm2reg);
    ByteStreamBuf buf(rm2reg, size);
    std::istream byteStream(&buf);
    InstStream instStream(&byteStream);

    Instruction inst;
    static u32 instCount = 0;
    while (inst = instStream.NextInstruction()) {
        ASSERT_TRUE(instCount < ARR_SIZE(expectedInsts));
        EXPECT_EQ(inst, expectedInsts[instCount])
            << "instruction mismatch, instruction index:"
            << instCount << std::endl;
        instCount++;
    }
}