#pragma once

#include <Windows.h>

namespace zaf {

class Message {
public:
    Message() : hwnd(nullptr), id(0), wParam(0), lParam(0) { }
    virtual ~Message() { }

public:
    HWND hwnd;
    UINT id;
    WPARAM wParam;
    LPARAM lParam;
};

}