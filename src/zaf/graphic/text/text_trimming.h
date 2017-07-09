#pragma once

#include <dwrite.h>
#include <cstdint>
#include <zaf/base/optional.h>
#include <zaf/graphic/text/text_inline_object.h>
#include <zaf/serialization/serializable_object.h>

namespace zaf {

class TextTrimming : public SerializableObject {
public:
    ZAF_DECLARE_TYPE_NAME();

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

protected:
    void SerializeToDataNode(DataNode& data_node) const override;
    bool DeserializeFromDataNode(const DataNode& data_node) override;
};


std::wstring ConvertTextTrimmingGranularityToString(TextTrimming::Granularity granularity);
zaf::optional<TextTrimming::Granularity> ConvertTextTrimmingGranularityFromString(const std::wstring& string);

}