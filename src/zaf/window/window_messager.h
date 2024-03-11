#pragma once

#include <zaf/base/non_copyable.h>
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

private:
    HWND window_handle_{};
};

}