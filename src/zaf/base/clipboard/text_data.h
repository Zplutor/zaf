#pragma once

#include <string_view>
#include <zaf/base/clipboard/clipboard_data.h>

namespace zaf {

class TextData : public ClipboardData {
public:
    TextData() = default;
    explicit TextData(std::wstring text);

    std::wstring_view Text() const {
        return text_;
    }

protected:
    DataMedium SaveToMedium(const DataFormat& format) override;
    void LoadFromMedium(const DataFormat& format, DataMedium medium) override;

private:
    std::wstring text_;
};

}