#pragma once

/**
@file
    Defines the class zaf::textual::StyledTextData;
*/

#include <zaf/clipboard/clipboard_data.h>
#include <zaf/control/textual/styled_text.h>

namespace zaf::textual {

/**
A data format to store StyledText in the clipboard.
*/
class StyledTextData : public clipboard::ClipboardData {
public:
    ZAF_DECLARE_TYPE;

    clipboard::Medium SaveToMedium(const clipboard::DataDescriptor& format) override;
    void LoadFromMedium(
        clipboard::FormatType format_type, 
        const clipboard::Medium& medium) override;

private:
    StyledText styled_text_;
};

}