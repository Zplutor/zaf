#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/clipboard/format.h>
#include <zaf/clipboard/medium.h>
#include <zaf/object/object.h>

namespace zaf::clipboard {

class ClipboardData : public Object, NonCopyableNonMovable {
public:
    ZAF_DECLARE_TYPE;

public:
    ClipboardData() = default;
    virtual ~ClipboardData() = default;

    virtual Medium SaveToMedium(const Format& format) = 0;
    virtual void LoadFromMedium(const Format& format, const Medium& medium) = 0;
};

}