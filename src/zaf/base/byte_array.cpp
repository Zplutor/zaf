#include <zaf/base/byte_array.h>

namespace zaf {

ByteArray::ByteArray(std::size_t size) : inner_(size) {

}


std::wstring ByteArray::ToHexString() const {

    static constexpr const wchar_t* hex_table = L"0123456789ABCDEF";

    std::wstring result;
    result.reserve(inner_.size() * 2);

    for (auto each_byte : inner_) {

        std::byte high = each_byte >> 4;
        result += hex_table[int(high)];

        std::byte low = (each_byte & std::byte(0xf));
        result += hex_table[int(low)];
    }

    return result;
}

}