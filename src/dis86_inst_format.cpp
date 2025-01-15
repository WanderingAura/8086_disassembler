#include <array>
#include <dis86_instruction_stream.h>
#include <cassert>
#include <iostream>
#include <unordered_map>

static const BitField BitLiteral(u8 bits, u8 size) {
    return {BitsUsage::Opcode, size, bits};
}

static const BitField D_BIT = {BitsUsage::Direction, 1}; 
static const BitField S_BIT = {BitsUsage::SignExt, 1};
static const BitField W_BIT = {BitsUsage::Width, 1};
static const BitField MOD_BITS = {BitsUsage::Mod, 2};
static const BitField REG_BITS = {BitsUsage::Reg, 3};
static const BitField SR_BITS = {BitsUsage::SR, 2};
static const BitField RM_BITS = {BitsUsage::RegMem, 3};

static const BitField HAS_DATA = {BitsUsage::HasData, 0, 1};
static const BitField WDATA_IF_W = {BitsUsage::WDataIfW, 0, 1};
static const BitField RM_IS_W = {BitsUsage::RMIsW, 0, 1};

static std::unordered_map<OpType, u8> BINARY_OP_LITS = {
    {OpType::ADD, 0},
    {OpType::OR, 1},
    {OpType::ADC, 2},
    {OpType::SBB, 3},
    {OpType::AND, 4},
    {OpType::SUB, 5},
    {OpType::XOR, 6},
    {OpType::CMP, 7},
};

enum class OpDirection {
    ModFirst = 0,
    RegFirst = 1
};

static const BitField DummyD(OpDirection val) {
    return {BitsUsage::Direction, 0, (u8)val};
}

static const BitField DummyW(u8 val) {
    return {BitsUsage::Width, 0, val};
}

static const BitField DummyReg(u8 val) {
    return {BitsUsage::Reg, 0, val};
}

static const BitField DummyRM(u8 val) {
    return {BitsUsage::RegMem, 0, val};
}

static const BitField DummyMod(u8 val) {
    return {BitsUsage::Mod, 0, val};
}

static const InstructionFormat RM2Reg(OpType type, BitField opField) {
    assert(opField.numBits == 6);
    return { type, {{ opField, D_BIT, W_BIT,
        MOD_BITS, REG_BITS, RM_BITS}} };
}

static const InstructionFormat MovImm2RM(OpType type, BitField opField) {
    return { type, {{ opField, W_BIT,
        MOD_BITS, BitLiteral(0b000, 3), RM_BITS,
        HAS_DATA, WDATA_IF_W, DummyD(OpDirection::ModFirst) }} };
}

static const InstructionFormat Imm2Reg(OpType type, BitField opField) {
    return { type, {{ opField, W_BIT, REG_BITS,
        HAS_DATA, WDATA_IF_W, DummyD(OpDirection::RegFirst) }} };
}

static const InstructionFormat MovMem2Acc() {
    return { OpType::MOV, {{ BitLiteral(0b1010000, 7), W_BIT,
        DummyMod(0b00), DummyReg(0b000), DummyRM(0b110),
        DummyD(OpDirection::RegFirst)}} };
}

static const InstructionFormat MovAcc2Mem() {
    return { OpType::MOV, {{ BitLiteral(0b1010001, 7), W_BIT,
        DummyMod(0b00), DummyReg(0b000), DummyRM(0b110),
        DummyD(OpDirection::ModFirst)}} };
}

static const InstructionFormat MovSR2RM() {
    return { OpType::MOV, {{ BitLiteral(0b100011, 6), D_BIT, BitLiteral(0b0, 1),
        MOD_BITS, BitLiteral(0b0, 1), SR_BITS, RM_BITS }} };
}

static const InstructionFormat OpImm2RM(OpType type, BitField opField) {
    assert(opField.numBits == 6);
    u8 lit = BINARY_OP_LITS.at(type);
    return { type, {{ opField, S_BIT, W_BIT,
        MOD_BITS, BitLiteral(lit, 3), RM_BITS,
        HAS_DATA, WDATA_IF_W, DummyD(OpDirection::ModFirst) }} };
}

static const InstructionFormat ImmOpAcc(OpType type, BitField opField) {
    assert(opField.numBits == 7);
    return { type, {{ opField, W_BIT, DummyReg(0b000), HAS_DATA, WDATA_IF_W,
        DummyD(OpDirection::RegFirst)}} };
}

static const InstructionFormat PushRM() {
    return { OpType::PUSH , {{ BitLiteral(0b11111111, 8),
        MOD_BITS, BitLiteral(0b110, 3), RM_BITS,
        DummyW(true)}} };
}

static const InstructionFormat PopRM() {
    return { OpType::POP , {{ BitLiteral(0b10001111, 8),
        MOD_BITS, BitLiteral(0b000, 3), RM_BITS,
        DummyW(true)}} };
}

static const InstructionFormat OpReg(OpType type, BitField opField) {
    return { type, {{ opField, REG_BITS, DummyW(true)}} };
}

static const InstructionFormat OpSR(OpType type, BitField opField1, BitField opField2) {
    return { type, {{ opField1, SR_BITS, opField2}} };
}

static const InstructionFormat PushSR() {
    return { OpType::PUSH, {{ BitLiteral(0b000, 3), SR_BITS, BitLiteral(0b110, 3) }} };
}

static const InstructionFormat PopSR() {
    return { OpType::POP, {{ BitLiteral(0b000, 3), SR_BITS, BitLiteral(0b111, 3) }} };
}

static const InstructionFormat XCHGRegRM() {
    return { OpType::XCHG, {{ BitLiteral(0b1000011, 7), W_BIT,
        MOD_BITS, REG_BITS, RM_BITS }} };
}

static const InstructionFormat XCHGRegAcc() {
    return { OpType::XCHG, {{ BitLiteral(0b10010, 5), REG_BITS, DummyW(true), DummyRM(0b000), DummyMod(0b11) }} };
}

static const InstructionFormat InPort2Acc() {
    return { OpType::IN, {{ BitLiteral(0b1110010, 7), W_BIT,
        HAS_DATA, DummyReg(0), DummyD(OpDirection::RegFirst) }} };
}

static const InstructionFormat OutAcc2Port() {
    return { OpType::OUT, {{ BitLiteral(0b1110011, 7), W_BIT,
        HAS_DATA, DummyReg(0), DummyD(OpDirection::ModFirst) }} };
}

static const InstructionFormat InDX2Acc() {
    return { OpType::IN, {{ BitLiteral(0b1110110, 7), W_BIT,
        DummyReg(0b000), DummyD(OpDirection::RegFirst),
        DummyMod(0b11), DummyRM(0b10), RM_IS_W }} };
}

static const InstructionFormat OutDX2Acc() {
    return { OpType::OUT, {{ BitLiteral(0b1110111, 7), W_BIT,
        DummyReg(0b000), DummyD(OpDirection::ModFirst),
        DummyMod(0b11), DummyRM(0b10), RM_IS_W }} };
}

static const InstructionFormat XLAT() {
    return { OpType::XLAT, {{ BitLiteral(0b11010111, 8) }} };
}

static const InstructionFormat LEA() {
    return { OpType::LEA, {{ BitLiteral(0b10001101, 8),
        MOD_BITS, REG_BITS, RM_BITS,
        DummyD(OpDirection::RegFirst), DummyW(true) }} };
}

static const InstructionFormat LDS() {
    return { OpType::LDS, {{ BitLiteral(0b11000101, 8),
        MOD_BITS, REG_BITS, RM_BITS,
        DummyD(OpDirection::RegFirst), DummyW(true) }} };
}

static const InstructionFormat LES() {
    return { OpType::LES, {{ BitLiteral(0b11000100, 8),
        MOD_BITS, REG_BITS, RM_BITS,
        DummyD(OpDirection::RegFirst), DummyW(true) }} };
}

const InstructionFormat InstStream::formats[] = {
    // mov instructions
    RM2Reg(OpType::MOV, BitLiteral(0b100010, 6)),
    MovImm2RM(OpType::MOV, BitLiteral(0b1100011, 7)),
    Imm2Reg(OpType::MOV, BitLiteral(0b1011, 4)),
    MovAcc2Mem(),
    MovMem2Acc(),
    MovSR2RM(),
    
    // add instructions
    RM2Reg(OpType::ADD, BitLiteral(0b000000, 6)),
    OpImm2RM(OpType::ADD, BitLiteral(0b100000, 6)),
    ImmOpAcc(OpType::ADD, BitLiteral(0b0000010, 7)),

    // adc instructions
    RM2Reg(OpType::ADC, BitLiteral(0b000100, 6)),
    OpImm2RM(OpType::ADC, BitLiteral(0b100000, 6)),
    ImmOpAcc(OpType::ADC, BitLiteral(0b0001010, 7)),
    // sub instructions
    RM2Reg(OpType::SUB, BitLiteral(0b001010, 6)),
    OpImm2RM(OpType::SUB, BitLiteral(0b100000, 6)),
    ImmOpAcc(OpType::SUB, BitLiteral(0b0010110, 7)),
    // sbb instructions
    RM2Reg(OpType::SBB, BitLiteral(0b000110, 6)),
    OpImm2RM(OpType::SBB, BitLiteral(0b100000, 6)),
    ImmOpAcc(OpType::SBB, BitLiteral(0b0001110, 7)),
    // cmp instructions
    RM2Reg(OpType::CMP, BitLiteral(0b001110, 6)),
    OpImm2RM(OpType::CMP, BitLiteral(0b100000, 6)),
    ImmOpAcc(OpType::CMP, BitLiteral(0b0011110, 7)),

    // push
    PushRM(),
    OpReg(OpType::PUSH, BitLiteral(0b01010, 5)),
    PushSR(),

    // pop
    PopRM(),
    OpReg(OpType::POP, BitLiteral(0b01011, 5)),
    PopSR(),
    
    // xchg
    XCHGRegRM(),
    XCHGRegAcc(),
    
    // in
    InPort2Acc(),
    InDX2Acc(),

    // out
    OutAcc2Port(),
    OutDX2Acc(),

    // other
    XLAT(),
    LEA(),
    LDS(),
    LES(),


};
