#pragma once

#include <zaf/control/event/click_info.h>
#include <zaf/control/textual_control.h>
#include <zaf/object/property_support.h>
#include <zaf/rx/observable.h>

namespace zaf {

/**
 Represents a clickable control.   

 This is the base class of all controls that support click action.
 */
class ClickableControl : public TextualControl {
public:
    ZAF_OBJECT;

public:
    ClickableControl();
    ~ClickableControl();

    /**
     Get a value indicating that whether the control is being pressed.
     */
    bool IsPressed() const {
        return is_pressed_;
    }

    /**
     Perform a click action on the control.
     */
    void Click();

    rx::Observable<ClickInfo> ClickEvent() const;

protected:
    void Initialize() override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;
    void OnMouseDown(const MouseDownInfo& event_info) override;
    void OnMouseUp(const MouseUpInfo& event_info) override;
    void OnMouseCaptured(const MouseCapturedInfo& event_info) override;
    void OnMouseReleased(const MouseReleasedInfo& event_info) override;
    void OnKeyDown(const KeyDownInfo& event_info) override;
    void OnKeyUp(const KeyUpInfo& event_info) override;

    /**
    Handles click notification. This method would be call when the control is clicked.

    The default implementation raises click event. Derived classes should call the same method of
    base class to raise the event if they override it.
    */
    virtual void OnClick(const ClickInfo& event_info);

private:
    enum class PressType {
        Mouse,
        Key,
    };

private:
    void BeginPress(PressType press_type);
    void EndPress(PressType press_type);
    bool CheckIsMousePressed(const Point& position, const MouseMessage& message);

private:
    bool is_pressed_;
    bool is_mouse_press_;
    bool is_key_press_;

    Event<ClickInfo> click_event_;
};

ZAF_OBJECT_BEGIN(ClickableControl);
ZAF_OBJECT_PROPERTY(IsPressed);
ZAF_OBJECT_END;

}