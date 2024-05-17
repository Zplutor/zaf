#pragma once

#include <string_view>
#include <zaf/clipboard/clipboard_data.h>

namespace zaf::clipboard {

class TextData : public ClipboardData {
public:
    ZAF_DECLARE_TYPE;

public:
    TextData() = default;
    explicit TextData(std::wstring text);

    const std::wstring& GetText() const {
        return text_;
    }

    std::wstring DetachText() {
        return std::move(text_);
    }

    Medium SaveToMedium(const DataDescriptor& data_descriptor) override;
    void LoadFromMedium(const Medium& medium, const DataDescriptor& data_descriptor) override;

private:
    std::wstring text_;
};

}