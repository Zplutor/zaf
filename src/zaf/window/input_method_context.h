#pragma once

#include <imm.h>
#include <zaf/base/non_copyable.h>
#include <zaf/graphic/font.h>
#include <zaf/graphic/point.h>

namespace zaf {

class Window;

class InputMethodContext : NonCopyable {
public:
    static InputMethodContext FromWindow(const Window& window);
    static InputMethodContext FromWindowHandle(HWND window_handle);
    
public:
    InputMethodContext() = default;
    InputMethodContext(HWND window_handle, HIMC context_handle);
    ~InputMethodContext();

    void MoveCompositionWindow(const Point& position_in_window);
    void SetCompositionFont(const Font& font);

    HIMC Handle() const noexcept {
        return context_handle_;
    }

private:
    HWND window_handle_{};
    HIMC context_handle_{};
};

}