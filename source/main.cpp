#include "rvi_read_binary.hpp"
#include <iomanip>
#include <iostream>

int main() {
    rvi::ReadBinary read_binary("tests/test");
    auto code_view = read_binary.GetTextSectionView();

    for (uint32_t i = 0; i < code_view.size() / 4; i++) {
        std::cout << std::hex << std::setfill('0') << std::setw(8) << reinterpret_cast<const uint32_t*>(code_view.data())[i] << "\n";
    }

    return 0;
}