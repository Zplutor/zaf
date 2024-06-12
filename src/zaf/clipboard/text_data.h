#pragma once

#include <string_view>
#include <zaf/clipboard/clipboard_data.h>

namespace zaf::clipboard {

class TextData : public ClipboardData {
public:
    TextData() = default;
    explicit TextData(std::wstring text);

    const std::wstring& Text() const {
        return text_;
    }

    [[nodiscard]]
    std::wstring Detach() {
        return std::move(text_);
    }

    Medium WriteToMedium(const DataDescriptor& data_descriptor) const override;
    void ReadFromMedium(const Medium& medium, const DataDescriptor& data_descriptor) override;

private:
    std::wstring text_;
};

}