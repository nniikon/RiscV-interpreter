#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace rvi {

class InterpreterMemoryModel {
private:
    const size_t kMemorySize = 1ull << 20;

private:
    std::vector<uint8_t> memory_;

public:
    InterpreterMemoryModel();

    template <typename T>
    T Get(uint32_t address) const;

    template <typename T>
    void Set(uint32_t address, T value);
};

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

} // namespace