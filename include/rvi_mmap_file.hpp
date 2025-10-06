#pragma once

#include <cstdint>
#include <span>
#include <string_view>

namespace rvi {

class MMapRO {
    std::uint8_t* ptr_ = nullptr;
    std::size_t   size_ = 0;


public:
    MMapRO(std::string_view path);
    ~MMapRO();

    MMapRO(const MMapRO&) = delete;
    MMapRO& operator=(const MMapRO&) = delete;

    MMapRO(MMapRO&& o);
    MMapRO& operator=(MMapRO&& o) = delete;

    std::size_t                   GetSize() const;
    std::span<const std::uint8_t> GetView() const;
};

} // namespace