#pragma once

#include <zaf/control/textual/dynamic_inline_object_events.h>
#include <zaf/control/textual/inline_object.h>

namespace zaf {
namespace internal {
class TextBoxInlineObjectManager;
class TextBoxMouseInputHandler;
}

namespace textual {

class DynamicInlineObject : public InlineObject {
public:
    ZAF_DECLARE_TYPE;

public:
    bool IsMouseOver() const;
    bool IsInSelectionRange() const;

protected:
    void NeedRepaint();

    void OnDetached(const DetachedInfo& event_info) override;

    virtual void OnMouseCursorChanging(const MouseCursorChangingInfo& event_info);
    virtual void OnMouseEnter(const MouseEnterInfo& event_info);
    virtual void OnMouseLeave(const MouseLeaveInfo& event_info);

    virtual void OnMouseDown();
    virtual void OnMouseUp();

private:
    friend class internal::TextBoxMouseInputHandler;

    internal::TextBoxInlineObjectManager* GetManager() const;

private:
    bool is_mouse_over_{};
};

}
}