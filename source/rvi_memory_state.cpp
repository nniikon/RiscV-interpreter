#include "rvi_memory_state.hpp"

#include <cstring>

using namespace rvi;

InterpreterMemoryModel::InterpreterMemoryModel()
    : memory_(kMemorySize) 
{}

template <typename T>
T InterpreterMemoryModel::Get(uint32_t address) const {
    T value{};
    std::memcpy(&value, &memory_[address], sizeof(T));
    return value;
}

template <typename T>
void InterpreterMemoryModel::Set(uint32_t address, T value) {
    std::memcpy(&memory_[address], &value, sizeof(T));
}