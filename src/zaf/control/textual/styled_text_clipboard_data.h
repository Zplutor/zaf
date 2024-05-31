#pragma once

/**
@file
    Defines the class zaf::textual::StyledTextClipboardData;
*/

#include <zaf/clipboard/clipboard_data.h>
#include <zaf/control/textual/styled_text.h>

namespace zaf::textual {

/**
A data format to store StyledText in the clipboard.
*/
class StyledTextClipboardData : public clipboard::ClipboardData {
public:
    static clipboard::FormatType StyledTextFormatType();

public:
    StyledTextClipboardData() = default;
    explicit StyledTextClipboardData(StyledText styled_text);

    clipboard::Medium SaveToMedium(
        const clipboard::DataDescriptor& data_descriptor) const override;

    void LoadFromMedium(
        const clipboard::Medium& medium,
        const clipboard::DataDescriptor& data_descriptor) override;

    [[nodiscard]]
    StyledText Detach() {
        return std::move(styled_text_);
    }

private:
    clipboard::Medium SaveToStyledTextFormat() const;

    static StyledText LoadStyledText(
        const clipboard::Medium& medium,
        const clipboard::DataDescriptor& data_descriptor);

    static StyledText LoadWithStyledTextFormat(const clipboard::Medium& medium);

private:
    StyledText styled_text_;
};

}