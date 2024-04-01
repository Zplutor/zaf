#include <zaf/base/base64.h>
#include <zaf/base/error/basic_error.h>

namespace zaf {
namespace {

constexpr int ByteBits = 8;
constexpr int UnitBits = 6;

constexpr wchar_t EncodeTable[] =
    L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    L"abcdefghijklmnopqrstuvwxyz"
    L"0123456789+/";

std::uint8_t DecodeChar(wchar_t ch) {

    if (L'A' <= ch && ch <= L'Z') {
        return ch - L'A';
    }

    if (L'a' <= ch && ch <= L'z') {
        return ch - L'a' + 26;
    }

    if (L'0' <= ch && ch <= L'9') {
        return ch - L'0' + 26 + 26;
    }

    if (ch == L'+') {
        return 62;
    }

    if (ch == L'/') {
        return 63;
    }

    ZAF_THROW_ERRC(BasicErrc::InvalidValue);
}

}

std::wstring Base64Encode(const void* data, std::size_t size) {

    std::wstring result;
    result.reserve(static_cast<std::size_t>(size * 1.4));

    auto begin = reinterpret_cast<const std::uint8_t*>(data);
    auto end = begin + size;

    std::uint8_t pending{};
    int pending_bits{};

    for (auto current = begin; current < end; ++current) {

        std::uint8_t unit = pending << (UnitBits - pending_bits);

        int consume_bits = UnitBits - pending_bits;
        pending_bits = ByteBits - consume_bits;

        pending = *current << consume_bits;
        pending >>= consume_bits;

        unit |= *current >> pending_bits;
        result += EncodeTable[unit];

        if (pending_bits == UnitBits) {
            result += EncodeTable[pending];
            pending = 0;
            pending_bits = 0;
        }
    }

    if (pending_bits != 0) {

        std::uint8_t unit = pending << (UnitBits - pending_bits);
        result += EncodeTable[unit];

        auto padding_count = 4 - result.length() % 4;
        result.append(padding_count, L'=');
    }

    return result;
}


std::vector<std::byte> Base64Decode(std::wstring_view encoded) {

    if (encoded.length() % 4 != 0) {
        ZAF_THROW_ERRC(BasicErrc::InvalidValue);
    }

    std::vector<std::byte> result;

    std::uint8_t pending{};
    int pending_bits{};

    auto current = encoded.begin();
    for (; current < encoded.end(); ++current) {

        if (*current == L'=') {
            ++current;
            break;
        }

        auto unit = DecodeChar(*current);

        if (pending_bits == 0) {

            pending = unit;
            pending_bits = UnitBits;
        }
        else {

            int consume_bits = ByteBits - pending_bits;
            pending_bits = UnitBits - consume_bits;

            std::uint8_t byte = pending << consume_bits;
            byte |= unit >> (UnitBits - consume_bits);
            result.push_back(static_cast<std::byte>(byte));

            pending = unit << pending_bits;
            pending >>= pending_bits;
        }
    }

    for (; current < encoded.end(); ++current) {
        if (*current != L'=') {
            ZAF_THROW_ERRC(BasicErrc::InvalidValue);
        }
    }

    return result;
}

}