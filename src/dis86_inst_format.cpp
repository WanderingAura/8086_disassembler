#include <array>
#include <dis86_instruction_stream.h>

static const BitField BitLiteral(u8 bits, u8 size) {
    return {BitsUsage::Opcode, size, bits};
}

static const BitField D_BIT = {BitsUsage::Direction, 1}; 
static const BitField W_BIT = {BitsUsage::Width, 1};
static const BitField MOD_BITS = {BitsUsage::Mod, 2};
static const BitField REG_BITS = {BitsUsage::Reg, 3};
static const BitField RM_BITS = {BitsUsage::RegMem, 3};

static const BitField HAS_DATA = {BitsUsage::HasData, 0, 1};
static const BitField WDATA_IF_W = {BitsUsage::WDataIfW, 0, 1};

enum class OpDirection {
    ModCommaReg = 0,
    RegCommaMod = 1
};

static const BitField DummyD(OpDirection val) {
    return {BitsUsage::Direction, 0, (u8)val};
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
    return { type, {{BitLiteral(0b100010, 6), D_BIT, W_BIT,
        MOD_BITS, REG_BITS, RM_BITS}} };
}

static const InstructionFormat Imm2RM(OpType type, BitField opField) {
    return { type, {{ BitLiteral(0b1100011, 7), W_BIT,
        MOD_BITS, BitLiteral(0b000, 3), RM_BITS,
        HAS_DATA, WDATA_IF_W, DummyD(OpDirection::ModCommaReg)}} };
}

static const InstructionFormat MovMem2Acc() {
    return { OpType::MOV, {{ BitLiteral(0b1010000, 7), W_BIT,
        DummyMod(0b00), DummyReg(0b000), DummyRM(0b110),
        DummyD(OpDirection::RegCommaMod)}} };
}

static const InstructionFormat MovAcc2Mem() {
    return { OpType::MOV, {{ BitLiteral(0b1010000, 7), W_BIT,
        DummyMod(0b00), DummyReg(0b000), DummyRM(0b110),
        DummyD(OpDirection::ModCommaReg)}} };
}

const InstructionFormat InstStream::formats[] = {
    // mov instructions
    RM2Reg(OpType::MOV, BitLiteral(0b100010, 6)),
    Imm2RM(OpType::MOV, BitLiteral(0b1100011, 7)),
    MovAcc2Mem(),
    MovMem2Acc(),
};