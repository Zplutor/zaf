#pragma once

#include <string_view>

namespace zaf::clipboard {

class Clipboard {
public:
    /**
    Gets unicode format text from clipboard.

    @return
        A string with unicode format.

    @throw zaf::Error
        Throws if fails to get unicode format text from clipboard.
    */
    static std::wstring GetText();

    /**
    Sets unicode format text to clipboard.

    @param text
        The text to be set to clipboard.

    @throw zaf::Error
        Throws if fails to set text to clipboard.
    */
    static void SetText(std::wstring_view text);
};

}