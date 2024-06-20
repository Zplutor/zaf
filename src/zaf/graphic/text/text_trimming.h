#pragma once

#include <dwrite.h>
#include <cstdint>
#include <zaf/graphic/text/text_inline_object.h>
#include <zaf/object/enum_declaration.h>
#include <zaf/object/enum_support.h>
#include <zaf/object/object.h>

namespace zaf {

enum class TextTrimmingGranularity {
    None = DWRITE_TRIMMING_GRANULARITY_NONE,
    Character = DWRITE_TRIMMING_GRANULARITY_CHARACTER,
    Word = DWRITE_TRIMMING_GRANULARITY_WORD,
};

ZAF_ENUM_BEGIN(TextTrimmingGranularity)
ZAF_ENUM_VALUE(None)
ZAF_ENUM_VALUE(Character)
ZAF_ENUM_VALUE(Word)
ZAF_ENUM_END;

ZAF_ENABLE_ENUM_BOXING(TextTrimmingGranularity);


class TextTrimming : public Object {
public:
    ZAF_OBJECT;

public:
    TextTrimmingGranularity Granularity() const {
        return static_cast<TextTrimmingGranularity>(inner_.granularity);
    }

    void SetGranularity(TextTrimmingGranularity granularity) {
        inner_.granularity = static_cast<DWRITE_TRIMMING_GRANULARITY>(granularity);
    }

    wchar_t Delimiter() const {
        return static_cast<wchar_t>(inner_.delimiter);
    }

    void SetDelimiter(wchar_t delimiter) {
        inner_.delimiter = delimiter;
    }

    std::size_t DelimiterCount() const {
        return static_cast<std::size_t>(inner_.delimiterCount);
    }

    void SetDelimiterCount(std::size_t count) {
        inner_.delimiterCount = static_cast<UINT32>(count);
    }

    const TextInlineObject& Sign() const {
        return sign_;
    }

    void SetSign(const TextInlineObject& sign) {
        sign_ = sign;
    }

    const DWRITE_TRIMMING& Inner() const {
        return inner_;
    }

    DWRITE_TRIMMING& Inner() {
        return inner_;
    }

private:
    DWRITE_TRIMMING inner_{};
    TextInlineObject sign_;
};

ZAF_OBJECT_BEGIN(TextTrimming);
ZAF_OBJECT_END;

}