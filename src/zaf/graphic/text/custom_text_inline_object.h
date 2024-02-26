#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/object/object.h>
#include <zaf/graphic/text/text_inline_object_metrics.h>

namespace zaf {

class Canvas;

class CustomTextInlineObject : public Object, NonCopyableNonMovable {
public:
    ZAF_DECLARE_TYPE;

public:
    CustomTextInlineObject() = default;

    virtual void Paint(Canvas& canvas) const;
    virtual TextInlineObjectMetrics GetMetrics() const;
};

}