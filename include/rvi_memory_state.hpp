#pragma once

#include <cstddef>
#include <cstdint>
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

} // namespace