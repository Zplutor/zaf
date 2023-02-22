#pragma once

#include <zaf/base/flag_enum.h>
#include <zaf/graphic/point.h>
#include <zaf/window/message/hit_test_result.h>
#include <zaf/window/message/message_shim.h>

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
class MouseMessage : public MessageShim {
public:
    using MessageShim::MessageShim;

    /**
     Get the mouse position in window's client area coordinate.
     */
    Point MousePosition() const;

    /**
     Get the mouse button that triggers this message.
     */
    MouseButton MouseButton() const;

    /**
     Get the mouse keys that is being pressed.
     */
    MouseKey PressedMouseKeys() const;

    /**
     Get hit test result of this message.
     */
    zaf::HitTestResult HitTestResult() const;
};


/**
 Wraps information of a Win32 mouse wheel message.
 */
class MouseWheelMessage : public MouseMessage {
public:
    using MouseMessage::MouseMessage;

    /**
     Get a value indicating that whether the wheeling is horizontal.
     */
    bool IsHorizontalWheeling() const {
        return (ID() == WM_MOUSEHWHEEL);
    }
    
    /**
     Get the wheeling distance.
     */
    int WheelingDistance() const {
        return static_cast<short>(HIWORD(WParam()));
    }
};

}