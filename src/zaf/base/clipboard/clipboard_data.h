#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/base/clipboard/data_format.h>
#include <zaf/base/clipboard/data_medium.h>

namespace zaf {

class ClipboardData : NonCopyableNonMovable {
public:
    ClipboardData() = default;
    virtual ~ClipboardData() = default;

    virtual DataMedium SaveToMedium(const DataFormat& format) = 0;
    virtual void LoadFromMedium(const DataFormat& format, DataMedium medium) = 0;
};

}