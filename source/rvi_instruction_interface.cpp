#include "rvi_instruction_interface.hpp"
#include "rvi_decode_info.hpp"

using namespace rvi;

IInstructionTypeR::IInstructionTypeR(uint32_t instr)
    : info_(DecodeInstructionTypeR(instr))
{}

IInstructionTypeI::IInstructionTypeI(uint32_t instr)
    : info_(DecodeInstructionTypeI(instr))
{}