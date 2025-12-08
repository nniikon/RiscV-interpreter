#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <type_traits>
#include <vector>

#include "loguru.hpp"

namespace rvi {

class InterpreterMemoryModel {
private:
    const size_t kMemorySize = 1ull << 32;

private:
    std::vector<uint8_t> memory_;

public:
    InterpreterMemoryModel();

    size_t Size() const noexcept { return memory_.size(); }

    template <typename T>
    T Get(uint32_t address) const;

    template <typename T>
    void Set(uint32_t address, T value);

    void LoadBytes(uint32_t address, std::span<const uint8_t> data) {
        if (data.empty()) {
            return;
        }
        std::memcpy(&memory_[address], data.data(), data.size());
    }

    template <typename T>
    T Read(uint32_t address) const noexcept {
        T value{};
        std::memcpy(&value, &memory_[address], sizeof(T));
        return value;
    }
};

template <typename T>
T InterpreterMemoryModel::Get(uint32_t address) const {
    DLOG_F(INFO, "Getting mem[%x] ...", address);

    T value{};
    std::memcpy(&value, &memory_[address], sizeof(T));

    {
        uint32_t debug_value = 0;
        std::memcpy(&debug_value, &value, sizeof(value));
        DLOG_F(INFO, "Getting mem[%x] = %x", address, debug_value);
    }

    return value;
}

template <typename T>
void InterpreterMemoryModel::Set(uint32_t address, T value) {
    {
        uint32_t debug_value = 0;
        std::memcpy(&debug_value, &value, sizeof(value));
        DLOG_F(INFO, "Setting mem[%x] = %x", address, debug_value);
    }

    std::memcpy(&memory_[address], &value, sizeof(T));
}

} // namespace
