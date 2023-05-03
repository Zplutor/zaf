#pragma once

namespace zaf {

class Clipboard {
public:
    /**
    Gets unicode format text from clipboard.

    @return
        A string with unicode format. If there is no specified format in clipboard, an empty string
        is returned.
    */
    static std::wstring GetText();
};

}