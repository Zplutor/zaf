#pragma once

#include <zaf/control/textual_control.h>
#include <zaf/rx/observable.h>

namespace zaf {

class ClickableControlClickInfo;

/**
 Represents a clickable control.   

 This is the base class of all controls that support click action.
 */
class ClickableControl : public TextualControl {
public:
    ZAF_DECLARE_TYPE

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

    Observable<ClickableControlClickInfo> ClickEvent();

protected:
    void Initialize() override;
    void OnMouseEnter(const MouseEnterInfo& event_info) override;
    void OnMouseLeave(const MouseLeaveInfo& event_info) override;
    bool OnMouseMove(const Point& position, const MouseMessage& message) override;
    bool OnMouseDown(const Point& position, const MouseMessage& message) override;
    bool OnMouseUp(const Point& position, const MouseMessage& message) override;
    void OnMouseCapture() override;
    void OnMouseRelease() override;
    void OnKeyDown(const KeyDownInfo& event_info) override;
    void OnKeyUp(const KeyUpInfo& event_info) override;
    void OnFocusChanged() override;

    virtual void OnClick();

private:
    enum class PressType {
        Mouse,
        Key,
    };

private:
    void BeginPress(PressType press_type);
    void EndPress(PressType press_type);
    void CheckIsMousePressed(const Point& position, const MouseMessage& message);

private:
    bool is_pressed_;
    bool is_mouse_press_;
    bool is_key_press_;
};


class ClickableControlClickInfo {
public:
    explicit ClickableControlClickInfo(const std::shared_ptr<ClickableControl>& control) : 
        control_(control) {

    }

    const std::shared_ptr<ClickableControl>& ClickableControl() const {
        return control_;
    }

private:
    std::shared_ptr<zaf::ClickableControl> control_;
};

}