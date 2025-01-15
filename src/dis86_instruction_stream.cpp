#include <dis86_num_types.h>
#include <fstream>
#include <iostream>
#include <dis86_instruction.h>
#include <dis86_instruction_stream.h>
#include <array>
#include <cassert>

#define MAX_FIELD_NUM 16
#define DIRECT_ADDRESS_IDX 8

inline Operand InstStream::GetRegOperand(u8 regVal, u8 widthVal) {
    Operand res = {};
    res.operandType = OperandType::REGISTER;
    res.reg.regIdx = (RegisterIdx)regVal;
    res.reg.isWide = widthVal;
    return res;
}

inline Operand InstStream::GetSegRegOperand(u8 regVal) {
    Operand res = {};
    res.operandType = OperandType::SEG_REG;
    res.reg.sRegIdx = (SegmentRegIdx)regVal;
    return res;
}

InstStream::InstStream(std::istream *binFile) {
    binFile->read((char *)bytes, ARR_SIZE(bytes));
    size = binFile->gcount();
    currentInstPointer = 0;
    readPointer = 0;
}

u8 InstStream::NextByte() {
    assert(readPointer < size);
    return bytes[readPointer++];
}

u16 InstStream::ParseData(bool isWide, bool isSignExt) {
    assert(isSignExt == 0 || isSignExt == 1);
    assert(isWide == 0 || isWide == 1);
    if (isWide) {
        u8 byte1 = NextByte();
        u8 byte2 = NextByte();
        assert(readPointer <= size);
        return (byte2 << 8) | byte1;
    }

    assert(readPointer <= size);
    // perform sign extension
    if (isSignExt)
        return (i16)(i8)NextByte();
    return NextByte();
}

void InstStream::GetBitFields(u32 &bitFieldFlags,
    std::array<u32, BitsUsage::NumElements>& bitFieldValues,
    const std::array<BitField, MAX_FIELD_NUM>& fields) {
    u8 bitsRemaining = 0;
    u8 currentByte = 0;
    for (BitField testField : fields) {
        if ((testField.name == BitsUsage::Opcode) && (testField.numBits == 0)) {
            // field is empty i.e. we have reached the end of the inst format.
            break;
        }

        u32 readVal = testField.val;
        if (testField.numBits != 0) {
            if (bitsRemaining == 0) {
                bitsRemaining = 8;
                currentByte = NextByte();
            }

            assert(testField.numBits <= bitsRemaining);

            bitsRemaining -= testField.numBits;

            readVal = (currentByte >> bitsRemaining);

            // set any used bits to zero
            currentByte &= (0xff >> (8 - bitsRemaining));
        }

        if (testField.name == BitsUsage::Opcode && testField.val != readVal) {
            // opcode does not match
            bitFieldFlags = 0;
            readPointer = currentInstPointer;
            return;
        } else {
            bitFieldValues[(u8)testField.name] = readVal;
            bitFieldFlags |= (1 << (u8)testField.name);
        }
    }
}

Instruction InstStream::TryDecode(const InstructionFormat format) {
    u32 bitFieldFlags = 0;
    std::array<u32, BitsUsage::NumElements> bitFieldValues = {};

    // TODO: this can be refactored into a class with all required instruction info
    // InstructionInfo::GetBitFields
    GetBitFields(bitFieldFlags, bitFieldValues, format.fields);

    if (bitFieldFlags == 0) {
        // instruction did not match given format
        return {};
    }

    u32 modVal = bitFieldValues[BitsUsage::Mod];
    u32 dirVal = bitFieldValues[BitsUsage::Direction];
    u32 widthVal = bitFieldValues[BitsUsage::Width];
    u32 signVal = bitFieldValues[BitsUsage::SignExt];
    u32 regVal = bitFieldValues[BitsUsage::Reg];
    u32 segRegVal = bitFieldValues[BitsUsage::SR];
    u32 regMemVal = bitFieldValues[BitsUsage::RegMem];

    bool hasDirectAddress = ((modVal == 0b00 && regMemVal == 0b110));
    bool hasDisp = (modVal == 0b01 || modVal == 0b10 || hasDirectAddress);
    bool dispIsW = (modVal == 0b10 || hasDirectAddress);
    bool hasData = bitFieldValues[BitsUsage::HasData];
    bool dataIsW = (bitFieldValues[BitsUsage::WDataIfW] && widthVal && !signVal);
    bool rmAlwaysW = bitFieldValues[BitsUsage::RMIsW];

    if (hasDisp)
        bitFieldValues[BitsUsage::Disp] = ParseData(dispIsW, true);
    if (hasData)
        bitFieldValues[BitsUsage::Data] = ParseData(dataIsW, signVal);
    
    i16 disp = bitFieldValues[BitsUsage::Disp];
    
    Operand operands[2] = {};
    Operand *regOperand = &operands[dirVal ? 0 : 1];
    Operand *modOperand = &operands[dirVal ? 1 : 0];

    if (bitFieldFlags & (1 << BitsUsage::SR)) {
        *regOperand = GetSegRegOperand(segRegVal);
    } else if (bitFieldFlags & (1 << BitsUsage::Reg)) {
        *regOperand = GetRegOperand(regVal, widthVal);
    }

    if (bitFieldFlags & (1 << BitsUsage::RegMem)) {
        if (modVal == 0b11) {
            *modOperand = GetRegOperand(regMemVal, widthVal | rmAlwaysW);
        } else {
            modOperand->operandType = OperandType::MEMORY;
            if (hasDirectAddress) {
                modOperand->address.expIdx = AddressExpIdx::DIRECT;
            } else {
                modOperand->address.expIdx = (AddressExpIdx)regMemVal;
            }
            modOperand->address.disp = disp;
        }
    }

    Operand *nextUnusedOp = (operands[0].operandType == OperandType::NONE) ?
        &operands[0] : &operands[1];
    if (nextUnusedOp->operandType != OperandType::NONE) {
        // both operands have been filled, return instruction
        return Instruction(format.op, operands[0], operands[1]);
    }

    if (hasData) {
        nextUnusedOp->operandType = OperandType::IMMEDIATE;
        nextUnusedOp->immediate.immU16 = bitFieldValues[BitsUsage::Data];
        nextUnusedOp->immediate.isWide = dataIsW;
    }

    return Instruction(format.op, operands[0], operands[1]);
}

Instruction InstStream::NextInstruction() {
    if (readPointer >= size) {
        return {};
    }
    for (const InstructionFormat& format : formats) {
        Instruction inst = TryDecode(format);
        if (inst) {
            currentInstPointer = readPointer;
            return inst;
        }
    }
    std::cerr << "failed to decode instruction" << std::endl;
    return {};
}