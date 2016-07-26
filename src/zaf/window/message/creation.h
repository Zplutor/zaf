#pragma once

#include <Windows.h>
#include <memory>

namespace zaf {

class Message;

/**
 Create a message wrapper with specified Win32 message information.

 The return value is returned as a base class. However, you can downcast it to a
 specified derived class according to the message id.
 */
std::shared_ptr<Message> CreateMessage(HWND hwnd, UINT id, WPARAM wParam, LPARAM lParam);

}