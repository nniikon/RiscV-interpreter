#include "rvi_mmap_file.hpp"

#include <string_view>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>

using namespace rvi;

MMapRO::MMapRO(std::string_view path) {
    int fd = open(path.data(), O_RDONLY);
    if (fd < 0) {
        throw std::runtime_error("Open failed");
    }

    struct stat st{};
    if (fstat(fd, &st) != 0) {
        close(fd);
        throw std::runtime_error("Stat failed");
    }

    if (!S_ISREG(st.st_mode)) {
        close(fd);
        throw std::runtime_error("Not a regular file");
    }

    std::size_t len = static_cast<std::size_t>(st.st_size);

    void* addr = mmap(nullptr, len, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        close(fd);
        throw std::runtime_error("Map failed");
    }

    close(fd);

    ptr_ = static_cast<std::uint8_t*>(addr);
    size_ = len;
}

MMapRO::~MMapRO() {
    if (ptr_) {
        munmap(ptr_, size_);
    }
}

MMapRO::MMapRO(MMapRO&& o) 
    : ptr_ (o.ptr_),
      size_(o.size_) {
    o.ptr_ = nullptr;
    o.size_ = 0;
}

std::size_t                   MMapRO::GetSize() const { return size_; }
std::span<const std::uint8_t> MMapRO::GetView() const { return {ptr_, size_}; }
