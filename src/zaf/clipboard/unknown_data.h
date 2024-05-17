#pragma once

#include <zaf/clipboard/clipboard_data.h>

namespace zaf::clipboard {

class UnknownData : public ClipboardData {
public:
    const clipboard::DataDescriptor& DataDescriptor() const {
        return data_descriptor_;
    }

    const clipboard::Medium& Medium() const {
        return medium_;
    }

    clipboard::Medium SaveToMedium(const clipboard::DataDescriptor& data_descriptor) override;

    void LoadFromMedium(
        const clipboard::Medium& medium, 
        const clipboard::DataDescriptor& data_descriptor) override;

private:
    clipboard::DataDescriptor data_descriptor_;
    clipboard::Medium medium_;
};

}