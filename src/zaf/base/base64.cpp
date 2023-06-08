#include <zaf/base/base64.h>
#include <zaf/base/error/check.h>

namespace zaf {
namespace {

constexpr int ByteBits = 8;
constexpr int UnitBits = 6;

constexpr const char* EncodeTable = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

}

std::string Base64Encode(const void* data, std::size_t size) {

    std::string result;
    result.reserve(static_cast<std::size_t>(size * 1.4));

    auto begin = reinterpret_cast<const std::uint8_t*>(data);
    auto end = begin + size;

    std::uint8_t pending{};
    int pending_bits{};

    for (auto current = begin; current < end; ++current) {

        int consume_bits = UnitBits - pending_bits;
        pending_bits = ByteBits - consume_bits;

        pending = *current << consume_bits;
        pending >>= consume_bits;

        std::uint8_t unit = *current >> pending_bits;
        result += EncodeTable[unit];

        if (pending_bits == UnitBits) {
            std::uint8_t next_unit = *current & 0b00111111;
            result += EncodeTable[next_unit];
            pending_bits = 0;
        }
    }

    if (pending_bits != 0) {

        std::uint8_t unit = pending << (UnitBits - pending_bits);
        result += EncodeTable[unit];

        auto padding_count = result.length() % 4;
        result.append(padding_count, '=');
    }

    return result;
}

}