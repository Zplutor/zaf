#pragma once

#include <Windows.h>
#include <memory>

namespace zaf {

class Message;

std::shared_ptr<Message> CreateMessage(HWND hwnd, UINT id, WPARAM wParam, LPARAM lParam);

}