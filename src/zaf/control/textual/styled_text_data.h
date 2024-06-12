#pragma once

/**
@file
    Defines the class zaf::textual::StyledTextData;
*/

#include <zaf/clipboard/clipboard_data.h>
#include <zaf/control/textual/styled_text.h>

namespace zaf::textual {

/**
A class reads and writes StyledText data in the clipboard.
*/
class StyledTextData : public clipboard::ClipboardData {
public:
    static clipboard::FormatType FormatType();

public:
    StyledTextData() = default;
    explicit StyledTextData(StyledText styled_text);

    clipboard::Medium WriteToMedium(
        const clipboard::DataDescriptor& data_descriptor) const override;

    void ReadFromMedium(
        const clipboard::Medium& medium,
        const clipboard::DataDescriptor& data_descriptor) override;

    [[nodiscard]]
    StyledText Detach() {
        return std::move(styled_text_);
    }

private:
    clipboard::Medium WriteToStyledTextFormat() const;

    static StyledText ReadStyledText(
        const clipboard::Medium& medium,
        const clipboard::DataDescriptor& data_descriptor);

    static StyledText ReadWithStyledTextFormat(const clipboard::Medium& medium);

private:
    StyledText styled_text_;
};

}