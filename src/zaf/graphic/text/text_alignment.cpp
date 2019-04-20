#include <zaf/graphic/text/text_alignment.h>

namespace zaf {
namespace {

const wchar_t* const Names[] = { 
    L"Leading",
    L"Tailing",
    L"Center",
    L"Justified" 
};

}

std::wstring ConvertTextAlignmentToString(TextAlignment text_alignment) {
    return Names[static_cast<int>(text_alignment)];
}

std::optional<TextAlignment> ConvertTextAlignmentFromString(const std::wstring& string) {
    for (int index = 0; index < sizeof(Names) / sizeof(wchar_t*); ++index) {
        if (string == Names[index]) {
            return static_cast<TextAlignment>(index);
        }
    }
    return { };
}

}