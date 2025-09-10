#include <zaf/base/byte_array.h>
#include <zaf/base/error/precondition_error.h>

namespace zaf {

ByteArray ByteArray::FromMemory(const void* data, std::size_t size) {

    ZAF_EXPECT(data);

    ByteArray result;
    if (size > 0) {
        result.Resize(size);
        std::memcpy(&result[0], data, size);
    }
    return result;
}


ByteArray ByteArray::FromString(std::string_view string) {
    return FromMemory(string.data(), string.size());
}


ByteArray ByteArray::FromString(std::wstring_view string) {
    return FromMemory(string.data(), string.length() * sizeof(wchar_t));
}


ByteArray::ByteArray(std::size_t size) : inner_(size) {

}


ByteArray::ByteArray(std::initializer_list<std::byte> bytes) : inner_(bytes) {

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