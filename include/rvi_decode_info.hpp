#pragma once

#include <cstdint>

namespace rvi {

struct InstructionDecodedInfoTypeR {
    uint32_t opcode;
    uint32_t rd;
    uint32_t funct3;
    uint32_t rs1;
    uint32_t rs2;
    uint32_t funct7;
};

struct InstructionDecodedInfoTypeI {
    uint32_t opcode;
    uint32_t rd;
    uint32_t funct3;
    uint32_t rs1;
    int32_t  imm;
};

struct InstructionDecodedInfoTypeS {
    uint32_t opcode;
    uint32_t funct3;
    uint32_t rs1;
    uint32_t rs2;
    int32_t  imm;
};

struct InstructionDecodedInfoTypeU {
    uint32_t opcode;
    uint32_t rd;
    int32_t  imm;
};

struct InstructionDecodedInfoTypeB {
    uint32_t opcode;
    uint32_t funct3;
    uint32_t rs1;
    uint32_t rs2;
    int32_t  imm;
};

struct InstructionDecodedInfoTypeJ {
    uint32_t opcode;
    uint32_t rd;
    int32_t  imm;
};

InstructionDecodedInfoTypeR DecodeInstructionTypeR(uint32_t instr);
InstructionDecodedInfoTypeI DecodeInstructionTypeI(uint32_t instr);
InstructionDecodedInfoTypeS DecodeInstructionTypeS(uint32_t instr);
InstructionDecodedInfoTypeU DecodeInstructionTypeU(uint32_t instr);
InstructionDecodedInfoTypeB DecodeInstructionTypeB(uint32_t instr);
InstructionDecodedInfoTypeJ DecodeInstructionTypeJ(uint32_t instr);

uint32_t EvalExtendedOpcode(InstructionDecodedInfoTypeR instr);
uint32_t EvalExtendedOpcode(InstructionDecodedInfoTypeI instr);
uint32_t EvalExtendedOpcode(InstructionDecodedInfoTypeS instr);
uint32_t EvalExtendedOpcode(InstructionDecodedInfoTypeU instr);
uint32_t EvalExtendedOpcode(InstructionDecodedInfoTypeB instr);
uint32_t EvalExtendedOpcode(InstructionDecodedInfoTypeJ instr);

} // namespace