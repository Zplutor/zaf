#include <zaf/control/text_validator.h>
#include <cwctype>

namespace zaf {

TextValidator GetNumberTextValidator() {

    return [](const RichEdit& rich_edit, const std::wstring& new_text) {

        for (auto each_char : new_text) {
            if (! std::iswdigit(each_char)) {
                return false;
            }
        }
        return true;
    };
}

}