#include <zaf/graphic/text/text_trimming.h>
#include <zaf/parsing/parsers/text_trimming_parser.h>
#include <zaf/object/type_definition.h>
#include <zaf/serialization/properties.h>

namespace zaf {
namespace {

const wchar_t* const Names[] = {
    L"None",
    L"Character",
    L"Word",
};

}

ZAF_DEFINE_TYPE(TextTrimming)
    ZAF_DEFINE_TYPE_PARSER(TextTrimmingParser)
ZAF_DEFINE_TYPE_END


std::wstring ConvertTextTrimmingGranularityToString(TextTrimming::Granularity granularity) {
    return Names[static_cast<std::size_t>(granularity)];
}


std::optional<TextTrimming::Granularity> ConvertTextTrimmingGranularityFromString(const std::wstring& string) {
    
    for (std::size_t index = 0; index < sizeof(Names) / sizeof(const wchar_t*); ++index) {
        if (string == Names[index]) {
            return static_cast<TextTrimming::Granularity>(index);
        }
    }

    return {};
}

}