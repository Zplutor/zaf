#pragma once

#include <zaf/base/flag_enum.h>
#include <zaf/graphic/point.h>
#include <zaf/window/message/hit_test_result.h>
#include <zaf/window/message/message.h>

namespace zaf {

/**
 Defines the mouse button.   
 */
enum class MouseButton {

    /**
     Mouse button is unspecified.
     */
    Unspecified,

    /**
     The left mouse button.
     */
    Left,

    /**
     The middle mouse button.
     */
    Middle,

    /**
     The right mouse button.
     */
    Right,
};


/**
 Defines mouse keys.
 */
enum class MouseKey {
    None = 0,
    LeftButton = MK_LBUTTON,
    MiddleButton = MK_MBUTTON,
    RightButton = MK_RBUTTON,
    XButton1 = MK_XBUTTON1,
    XButton2 = MK_XBUTTON2,
    Shift = MK_SHIFT,
    Control = MK_CONTROL,
};

ZAF_ENABLE_FLAG_ENUM(MouseKey);


/**
 Wraps information of a Win32 mouse message.
 */
class MouseMessage : public Message {
public:
    /**
     Get the mouse position in window's client area coordinate.
     */
    Point GetMousePosition() const;

    /**
     Get the mouse button that triggers this message.
     */
    MouseButton GetMouseButton() const;

    /**
     Get the mouse keys that is being pressed.
     */
    MouseKey GetPressedMouseKeys() const;

    /**
     Get hit test result of this message.
     */
    HitTestResult GetHitTestResult() const;
};


/**
 Wraps information of a Win32 mouse wheel message.
 */
class MouseWheelMessage : public MouseMessage {
public:
    /**
     Get a value indicating that whether the wheeling is horizontal.
     */
    bool IsHorizontalWheeling() const {
        return (id == WM_MOUSEHWHEEL);
    }
    
    /**
     Get the wheeling distance.
     */
    int GetWheelingDistance() const {
        return static_cast<short>(HIWORD(wparam));
    }
};

}