#include <zaf/base/base64.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/base/error/check.h>

namespace zaf {
namespace {

constexpr int ByteBits = 8;
constexpr int UnitBits = 6;

constexpr char EncodeTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

std::uint8_t DecodeChar(char ch) {

    if ('A' <= ch && ch <= 'Z') {
        return ch - 'A';
    }

    if ('a' <= ch && ch <= 'z') {
        return ch - 'a' + 26;
    }

    if ('0' <= ch && ch <= '9') {
        return ch - '0' + 26 + 26;
    }

    if (ch == '+') {
        return 62;
    }

    if (ch == '/') {
        return 63;
    }

    ZAF_THROW_ERRC(BasicErrc::InvalidValue);
}

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


std::vector<std::byte> Base64Decode(std::string_view encoded) {

    if (encoded.length() % 4 != 0) {
        ZAF_THROW_ERRC(BasicErrc::InvalidValue);
    }

    std::vector<std::byte> result;

    std::uint8_t pending{};
    int pending_bits{};

    auto current = encoded.begin();
    for (; current < encoded.end(); ++current) {

        if (*current == '=') {
            ++current;
            break;
        }

        auto decoded_byte = DecodeChar(*current);

        if (pending_bits == 0) {

            pending = decoded_byte;
            pending_bits = UnitBits;
        }
        else if (pending_bits == 6) {

            std::uint8_t byte = pending << (ByteBits - pending_bits);
            byte |= decoded_byte >> (ByteBits - pending_bits);
            result.push_back(static_cast<std::byte>(byte));

            pending_bits = (ByteBits - pending_bits);

            pending = decoded_byte << (ByteBits - pending_bits);
            pending >>= (ByteBits - pending_bits);
        }
        else if (pending_bits == 4) {

            std::uint8_t byte = pending << 4;
            byte |= (decoded_byte & 0b00111100) >> 2;
            result.push_back(static_cast<std::byte>(byte));

            pending = (decoded_byte & 0b00000011);
            pending_bits = 2;
        }
        else if (pending_bits == 2) {

            std::uint8_t byte = pending << 6;
            byte |= (decoded_byte & 0b00111111);
            result.push_back(static_cast<std::byte>(byte));

            pending = 0;
            pending_bits = 0;
        }
    }

    if (pending_bits != 0 && pending != 0) {
        ZAF_THROW_ERRC(BasicErrc::InvalidValue);
    }

    for (; current < encoded.end(); ++current) {
        if (*current != '=') {
            ZAF_THROW_ERRC(BasicErrc::InvalidValue);
        }
    }

    return result;
}

}