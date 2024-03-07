#pragma once

#include <zaf/control/textual/inline_object.h>
#include <zaf/control/textual/inline_object_events.h>

namespace zaf::textual {

class DynamicInlineObject : public InlineObject {
public:
    ZAF_DECLARE_TYPE;

public:
    bool IsMouseOver() const;
    bool IsInSelectionRange() const;

protected:
    virtual void OnMouseCursorChanging(const textual::MouseCursorChangingInfo& event_info);
    virtual void OnMouseDown();
    virtual void OnMouseUp();
};

}