#pragma once

#include <vector>
#include <zaf/base/clipboard/data_format.h>
#include <zaf/base/clipboard/clipboard_data.h>

namespace zaf::internal {

class DataFormatItem {
public:
    DataFormatItem(const DataFormat& format, std::shared_ptr<ClipboardData> data) :
        format_(format),
        data_(std::move(data)) {

    }

    const DataFormat& Format() const {
        return format_;
    }

    const std::shared_ptr<ClipboardData>& Data() const {
        return data_;
    }

private:
    DataFormat format_;
    std::shared_ptr<ClipboardData> data_;
};

using DataFormatItemList = std::vector<DataFormatItem>;

}