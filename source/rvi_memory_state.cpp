#include "rvi_memory_state.hpp"

#include <cstring>

using namespace rvi;

InterpreterMemoryModel::InterpreterMemoryModel()
    : memory_(kMemorySize) 
{}