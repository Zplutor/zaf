#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/graphic/text/text_inline_object_metrics.h>

namespace zaf {

class Canvas;

class CustomTextInlineObject : NonCopyableNonMovable {
public:
    CustomTextInlineObject() = default;
    virtual ~CustomTextInlineObject() = default;

    virtual void Paint(Canvas& canvas) = 0;
    virtual TextInlineObjectMetrics GetMetrics() = 0;
};

}