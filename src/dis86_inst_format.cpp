#include <array>
#include <dis86_instruction_stream.h>

#define OP_FIELD(Bits) {BitsUsage::Opcode, sizeof(#Bits)-1, 0b##Bits}

#define D {BitsUsage::Direction, 1}
#define W {BitsUsage::Width, 1}

#define RM {BitsUsage::RegMem, 3}
#define MOD {BitsUsage::Mod, 2}
#define REG {BitsUsage::Reg, 3}

#define DUMMY_W {BitsUsage::Width, 0, 1}
#define DUMMY_D(value) {BitsUsage::Direction, 0, value}
#define DUMMY_REG(Value) {BitsUsage::Reg, 0, Value}
#define DUMMY_MOD(Value) {BitsUsage::Mod, 0, Value}
#define DUMMY_RM(Value) {BitsUsage::RegMem, 0, Value}

#define DATA {BitsUsage::HasData, 0, 1}
#define DATA_IF_W {BitsUsage::WDataIfW, 0, 1}

const InstructionFormat InstStream::formats[] = {
    { OpType::MOV, {{OP_FIELD(100010), D, W, MOD, REG, RM}} },
    { OpType::MOV, {{OP_FIELD(1100011), W, MOD, OP_FIELD(000), RM, DATA, DATA_IF_W, DUMMY_D(0)}} }
};