#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/input/key.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {

/**
An utility class used to send or post messages to a specified window.

This class is designed primarily to avoid conflict with SendMessage and PostMessage macros, 
without defining SendMessage() and PostMessage() directly in the Window class.
*/
class WindowMessager : NonCopyable {
public:
    explicit WindowMessager(HWND window_handle);

    /**
    Sends the specified message to the window.

    @param message_id
        The message to be sent.

    @param wparam
        Additional message-specific information.

    @param lparam
        Additional message-specific information.

    @return
        The return value specifies the result of the message processing.
    */
    LRESULT Send(UINT message_id, WPARAM wparam, LPARAM lparam) noexcept;

    /**
    Posts the specified message to the window.

    @param message_id
        The message to be posted.

    @param wparam
        Additional message-specific information.

    @param lparam
        Additional message-specific information.

    @throw zaf::Error
        Thrown if posting the message fails.
    */
    void Post(UINT message_id, WPARAM wparam, LPARAM lparam);

    /**
    Posts a WM_SETCURSOR message to the window, using information from the specified mouse message.

    @param mouse_message
        The mouse message containing information used to populate the WM_SETCURSOR message.

    @throw zaf::Error
        Thrown if the operation fails.
    */
    void PostWMSETCURSOR(const MouseMessage& mouse_message);

    /**
    Sends a WM_LBUTTONDOWN message with the specified position to the window.

    @param position
        The position set to the LPARAM parameter of the WM_LBUTTONDOWN message. It is expressed in 
        the coordinate space of the window's client area and is in DIPs.

    @return
        The result of the message processing.

    @details
        The WPARAM parameter of the message is set to MK_LBUTTON.
    */
    LRESULT SendWMLBUTTONDOWN(const Point& position);

    /**
    Sends a WM_LBUTTONUP message with the specified position to the window.

    @param position
        The position set to the LPARAM parameter of the WM_LBUTTONUP message. It is expressed in
        the coordinate space of the window's client area and is in DIPs.

    @return
        The result of the message processing.

    @details
        The WPARAM parameter of the message is set to 0.
    */
    LRESULT SendWMLBUTTONUP(const Point& position);

    /**
    Sends a WM_RBUTTONDOWN message with the specified position to the window.

    @param position
        The position set to the LPARAM parameter of the WM_RBUTTONDOWN message. It is expressed in
        the coordinate space of the window's client area and is in DIPs.

    @return
        The result of the message processing.

    @details
        The WPARAM parameter of the message is set to MK_RBUTTON.
    */
    LRESULT SendWMRBUTTONDOWN(const Point& position);

    /**
    Sends a WM_RBUTTONUP message with the specified position to the window.

    @param position
        The position set to the LPARAM parameter of the WM_RBUTTONUP message. It is expressed in
        the coordinate space of the window's client area and is in DIPs.

    @return
        The result of the message processing.

    @details
        The WPARAM parameter of the message is set to 0.
    */
    LRESULT SendWMRBUTTONUP(const Point& position);

    /**
    Sends a WM_MOUSEMOVE message with the specified position to the window.

    @param position
        The position set to the LPARAM parameter of the WM_MOUSEMOVE message. It is expressed in
        the coordinate space of the window's client area and is in DIPs.

    @return
        The result of the message processing.

    @details
        The WPARAM parameter of the message is set to 0.
    */
    LRESULT SendWMMOUSEMOVE(const Point& position);

    /**
    Sends a WM_KEYDOWN message with the specified key to the window.

    @param key
        The key set to the WPARAM parameter of the WM_KEYDOWN message.

    @return
        The result of the message processing.
    */
    LRESULT SendWMKEYDOWN(Key key);

private:
    LPARAM ToLPARAM(const Point& point) const;

private:
    HWND window_handle_{};
};

}