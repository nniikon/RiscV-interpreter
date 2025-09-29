#pragma once

#include <cstddef>

namespace rvi {

const size_t kNumRegs = 32;

enum class InstructionType {
    unknown = 0,
    r,
    i,
    s,
    u,
};

} // namespace