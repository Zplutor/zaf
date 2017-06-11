#include <zaf/graphic/text/word_wrapping.h>

namespace zaf {
namespace {

const wchar_t* const Names[] = {
    L"Wrap",
    L"NoWrap",
    L"EmergencyBreak",
    L"WholeWord",
    L"Character",
};

}

std::wstring ConvertWordWrappingToString(WordWrapping word_wrapping) {
    return Names[static_cast<int>(word_wrapping)];
}

optional<WordWrapping> ConvertWordWrappingFromString(const std::wstring& string) {
    for (int index = 0; index < sizeof(Names) / sizeof(wchar_t*); ++index) {
        if (string == Names[index]) {
            return static_cast<WordWrapping>(index);
        }
    }
    return { };
}

}