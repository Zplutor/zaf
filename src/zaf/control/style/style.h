#pragma once

#include <zaf/object/object.h>

namespace zaf {

class Style : public Object, NonCopyableNonMovable {
public:
    ZAF_OBJECT;

    virtual void ApplyTo(Object& object) = 0;
};

ZAF_OBJECT_BEGIN(Style);
ZAF_OBJECT_END;

}