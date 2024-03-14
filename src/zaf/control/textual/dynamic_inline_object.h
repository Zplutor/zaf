#pragma once

#include <zaf/control/textual/dynamic_inline_object_events.h>
#include <zaf/control/textual/inline_object.h>

namespace zaf {

class TextBox;

namespace internal {
class TextBoxInlineObjectManager;
class TextBoxMouseInputHandler;
}

namespace textual {

class DynamicInlineObject : public InlineObject {
public:
    ZAF_DECLARE_TYPE;

public:
    std::shared_ptr<TextBox> Host() const noexcept;

    bool IsMouseOver() const;
    bool IsInSelectionRange() const;

protected:
    void NeedRepaint();

    void OnDetached(const DetachedInfo& event_info) override;

    /**
    Performs a hit test to determine whether the inline object should respond to mouse input.

    @param is_mouse_inside
        A value indicating whether the mouse cursor is inside the boundary of the inline object.

    @return
        Returns true if the inline object should respond to the mouse input, false otherwise. 

    @remark
        This method is called to determine whether the inline object should respond to the mouse
        input when the mouse cursor is over the area which is considered to be the interactive area
        of an inline object. The interactive area of an inline object may be larger than the 
        boundary of the object. For example, in a single-line text box, an inline object could
        respond to mouse inputs even if the mouse cursor is above the top or beneath the bottom of 
        the object. 
        
        The default implementation returns true. A derived class that wants to respond to mouse 
        input only if the mouse cursor is inside the boundary of the object can override this 
        method and returns a desired value according to is_mouse_inside.
    */
    virtual bool HitTest(bool is_mouse_inside);

    virtual void OnMouseCursorChanging(const MouseCursorChangingInfo& event_info);
    virtual void OnMouseEnter(const MouseEnterInfo& event_info);
    virtual void OnMouseLeave(const MouseLeaveInfo& event_info);

    virtual void OnMouseDown(const MouseDownInfo& event_info);
    virtual void OnMouseUp(const MouseUpInfo& event_info);

private:
    friend class internal::TextBoxMouseInputHandler;

private:
    bool is_mouse_over_{};
};

}
}