#pragma once

#include <zaf/clipboard/clipboard_data.h>

namespace zaf::clipboard {

class UnknownData : public ClipboardData {
public:
    const clipboard::Format& Format() const {
        return format_;
    }

    const clipboard::Medium& Medium() const {
        return medium_;
    }

    clipboard::Medium SaveToMedium(const clipboard::Format& format) override;
    void LoadFromMedium(const clipboard::Format& format, const clipboard::Medium& medium) override;

private:
    clipboard::Format format_;
    clipboard::Medium medium_;
};

}