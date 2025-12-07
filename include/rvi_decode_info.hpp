#pragma once

#include <cstdint>
#include <variant>

namespace rvi {

enum class InstructionType {
    unknown = 0,
    R,
    R4,
    I,
    S,
    U,
};

struct InstructionDecodedInfoTypeR {
    uint32_t opcode;
    uint32_t rd;
    uint32_t funct3;
    uint32_t rs1;
    uint32_t rs2;
    uint32_t funct7;
};

struct InstructionDecodedInfoTypeR4 {
    uint32_t opcode;
    uint32_t rd;
    uint32_t rm; 
    uint32_t rs1;
    uint32_t rs2;
    uint32_t rs3;
    uint32_t fmt;
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

using InstructionDecodedCommonType =
      std::variant<InstructionDecodedInfoTypeR,
                   InstructionDecodedInfoTypeI,
                   InstructionDecodedInfoTypeB,
                   InstructionDecodedInfoTypeJ,
                   InstructionDecodedInfoTypeU,
                   InstructionDecodedInfoTypeS,
                   InstructionDecodedInfoTypeR4>;

InstructionDecodedInfoTypeR DecodeInstructionTypeR(uint32_t instr);
InstructionDecodedInfoTypeI DecodeInstructionTypeI(uint32_t instr);
InstructionDecodedInfoTypeS DecodeInstructionTypeS(uint32_t instr);
InstructionDecodedInfoTypeU DecodeInstructionTypeU(uint32_t instr);
InstructionDecodedInfoTypeB DecodeInstructionTypeB(uint32_t instr);
InstructionDecodedInfoTypeJ DecodeInstructionTypeJ(uint32_t instr);
InstructionDecodedInfoTypeR4 DecodeInstructionTypeR4(uint32_t instr);

InstructionDecodedCommonType DecodeInstructionToCommonTypeR(uint32_t instr);
InstructionDecodedCommonType DecodeInstructionToCommonTypeI(uint32_t instr);
InstructionDecodedCommonType DecodeInstructionToCommonTypeS(uint32_t instr);
InstructionDecodedCommonType DecodeInstructionToCommonTypeU(uint32_t instr);
InstructionDecodedCommonType DecodeInstructionToCommonTypeB(uint32_t instr);
InstructionDecodedCommonType DecodeInstructionToCommonTypeJ(uint32_t instr);
InstructionDecodedCommonType DecodeInstructionToCommonTypeR4(uint32_t instr);

} // namespace
