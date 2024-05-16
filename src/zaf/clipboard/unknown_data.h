#pragma once

#include <zaf/clipboard/clipboard_data.h>

namespace zaf::clipboard {

class UnknownData : public ClipboardData {
public:
    FormatType FormatType() const {
        return format_type_;
    }

    const clipboard::Medium& Medium() const {
        return medium_;
    }

    clipboard::Medium SaveToMedium(const clipboard::DataDescriptor& format) override;
    void LoadFromMedium(
        clipboard::FormatType format_type, 
        const clipboard::Medium& medium) override;

private:
    clipboard::FormatType format_type_{ FormatType::Indeterminate };
    clipboard::Medium medium_;
};

}