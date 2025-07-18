#pragma once

#include <zaf/base/event/event.h>
#include <zaf/control/textual/inline_object.h>
#include <zaf/control/textual/active_inline_object_events.h>

namespace zaf {

class TextBox;

namespace internal {
class TextBoxInlineObjectManager;
class TextBoxMouseInputHandler;
}

namespace textual {

class ActiveInlineObject : public InlineObject {
public:
    ZAF_OBJECT;

public:
    std::shared_ptr<TextBox> Host() const noexcept;

    std::optional<Point> PositionInHost() const;
    bool IsMouseOver() const noexcept;
    bool IsInSelectionRange() const noexcept;

    rx::Observable<InlineObjectMouseEnterInfo> MouseEnterEvent() const {
        return mouse_enter_event_.GetObservable();
    }

    rx::Observable<InlineObjectMouseLeaveInfo> MouseLeaveEvent() const {
        return mouse_leave_event_.GetObservable();
    }

    rx::Observable<InlineObjectMouseDownInfo> MouseDownEvent() const {
        return mouse_down_event_.GetObservable();
    }

    rx::Observable<InlineObjectMouseUpInfo> MouseUpEvent() const {
        return mouse_up_event_.GetObservable();
    }

    rx::Observable<InlineObjectDoubleClickInfo> DoubleClickEvent() const {
        return double_click_event_.GetObservable();
    }

    void WriteToXML(XMLWriter& writer) const override;
    void ReadFromXML(XMLReader& reader) override;

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

    virtual void OnMouseCursorChanging(const InlineObjectMouseCursorChangingInfo& event_info);
    virtual void OnMouseEnter(const InlineObjectMouseEnterInfo& event_info);
    virtual void OnMouseLeave(const InlineObjectMouseLeaveInfo& event_info);

    virtual void OnMouseDown(const InlineObjectMouseDownInfo& event_info);
    virtual void OnMouseUp(const InlineObjectMouseUpInfo& event_info);

    virtual void OnDoubleClick(const InlineObjectDoubleClickInfo& event_info);

private:
    friend class internal::TextBoxMouseInputHandler;

private:
    bool is_mouse_over_{};

    Event<InlineObjectMouseEnterInfo> mouse_enter_event_;
    Event<InlineObjectMouseLeaveInfo> mouse_leave_event_;
    Event<InlineObjectMouseDownInfo> mouse_down_event_;
    Event<InlineObjectMouseUpInfo> mouse_up_event_;
    Event<InlineObjectDoubleClickInfo> double_click_event_;
};

ZAF_OBJECT_BEGIN(ActiveInlineObject);
ZAF_OBJECT_END;

}
}