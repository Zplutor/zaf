#include <zaf/control/check_state.h>

namespace zaf {
namespace {

const wchar_t* const Names[] = {
    L"Unchecked",
    L"Checked",
    L"Indeterminate",
};

}

std::wstring ConvertCheckStateToString(CheckState check_state) {
    return Names[static_cast<std::size_t>(check_state)];
}


optional<CheckState> ConvertCheckStateFromString(const std::wstring& string) {

    for (std::size_t index = 0; index < sizeof(Names) / sizeof(const wchar_t*); ++index) {
        if (string == Names[index]) {
            return static_cast<CheckState>(index);
        }
    }

    return {};
}

}