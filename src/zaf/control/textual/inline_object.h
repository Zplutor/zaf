#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/textual/inline_object_events.h>
#include <zaf/object/object.h>
#include <zaf/graphic/text/text_inline_object_metrics.h>

namespace zaf {

class Canvas;

namespace textual {

class InlineObject : public Object, NonCopyableNonMovable {
public:
    ZAF_DECLARE_TYPE;

public:
    InlineObject() = default;

    virtual void Paint(Canvas& canvas) const;
    virtual TextInlineObjectMetrics GetMetrics() const;

protected:
    virtual void OnMouseCursorChanging(const textual::MouseCursorChangingInfo& event_info);
    virtual void OnMouseDown();
    virtual void OnMouseUp();
};

}
}