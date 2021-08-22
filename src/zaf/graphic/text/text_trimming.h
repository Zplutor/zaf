#pragma once

#include <dwrite.h>
#include <cstdint>
#include <optional>
#include <zaf/graphic/text/text_inline_object.h>
#include <zaf/object/object.h>

namespace zaf {

class TextTrimming : public Object {
public:
    ZAF_DECLARE_TYPE

public:
    enum class Granularity {
        None = DWRITE_TRIMMING_GRANULARITY_NONE,
        Character = DWRITE_TRIMMING_GRANULARITY_CHARACTER,
        Word = DWRITE_TRIMMING_GRANULARITY_WORD,
    };

public:
    Granularity granularity = Granularity::None;
    wchar_t delimiter = 0;
    std::uint32_t delimiter_count = 0;
    TextInlineObject trimming_sign;
};


std::wstring ConvertTextTrimmingGranularityToString(TextTrimming::Granularity granularity);
std::optional<TextTrimming::Granularity> ConvertTextTrimmingGranularityFromString(const std::wstring& string);

}