#include <zaf/graphic/text/paragraph_alignment.h>

namespace zaf {
namespace {

const wchar_t* const Names[] = {
    L"Near",
    L"Far",
    L"Center",
};

}


std::wstring ConvertParagraphAlignmentToString(ParagraphAlignment paragraph_alignment) {
    return Names[static_cast<int>(paragraph_alignment)];
}


std::optional<ParagraphAlignment> ConvertParagraphAlignmentFromString(const std::wstring& string) {
    for (std::size_t index = 0; index < sizeof(Names) / sizeof(wchar_t*); ++index) {
        if (string == Names[index]) {
            return static_cast<ParagraphAlignment>(index);
        }
    }
    return { };
}

}