#pragma once

#include <vector>
#include <zaf/clipboard/format.h>
#include <zaf/clipboard/medium.h>

namespace zaf::clipboard::internal {

class FormatItem {
public:
    FormatItem(const Format& format, Medium medium) :
        format_(format),
        medium_(std::move(medium)) {

    }

    const Format& Format() const {
        return format_;
    }

    const Medium& Medium() const {
        return medium_;
    }

private:
    clipboard::Format format_;
    clipboard::Medium medium_;
};

using FormatItemList = std::vector<FormatItem>;

}