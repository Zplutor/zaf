#pragma once

#include <zaf/graphic/point.h>
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
 Wraps information of a Win32 mouse message.
 */
class MouseMessage : public Message {
public:
    /**
     Get the mouse position in window's client rect coordinate.
     */
    virtual const Point GetMousePosition() const;

    /**
     Get the mouse button.
     */
    MouseButton GetMouseButton() const;
};


/**
 Wraps information of a Win32 mouse wheel message.
 */
class MouseWheelMessage : public MouseMessage {
public:
    const Point GetMousePosition() const override;

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
        return static_cast<short>(HIWORD(wParam));
    }
};

}