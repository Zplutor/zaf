#pragma once

#include <vector>
#include <zaf/clipboard/data_descriptor.h>
#include <zaf/clipboard/medium.h>

namespace zaf::clipboard::internal {

class FormatItem {
public:
    FormatItem(const DataDescriptor& format, Medium medium) :
        format_(format),
        medium_(std::move(medium)) {

    }

    const DataDescriptor& DataDescriptor() const {
        return format_;
    }

    const Medium& Medium() const {
        return medium_;
    }

private:
    clipboard::DataDescriptor format_;
    clipboard::Medium medium_;
};

using FormatItemList = std::vector<FormatItem>;

}