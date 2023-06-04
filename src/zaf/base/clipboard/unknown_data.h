#pragma once

#include <zaf/base/clipboard/clipboard_data.h>
#include <zaf/base/error/basic_error.h>

namespace zaf {

class UnknownData : public ClipboardData {
public:
    UnknownData() = default;

    const DataMedium& Medium() const {
        return medium_;
    }

protected:
    DataMedium SaveToMedium(const DataFormat& format) override {
        ZAF_THROW_ERRC(BasicErrc::InvalidOperation);
    }

    void LoadFromMedium(const DataFormat& format, DataMedium medium) override {
        medium_ = std::move(medium);
    }

private:
    DataMedium medium_;
};

}