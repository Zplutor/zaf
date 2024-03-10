#pragma once

#include <zaf/control/textual/dynamic_inline_object_events.h>
#include <zaf/control/textual/inline_object.h>

namespace zaf {
namespace internal {
class TextBoxInlineObjectManager;
}

namespace textual {

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

private:
    internal::TextBoxInlineObjectManager* GetManager() const;
};

}
}