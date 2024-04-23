#include <zaf/window/input_method_context.h>
#include <zaf/base/error/contract.h>
#include <zaf/graphic/dpi.h>
#include <zaf/window/window.h>

namespace zaf {

InputMethodContext InputMethodContext::FromWindow(const Window& window) {
    return FromWindowHandle(window.Handle());
}


InputMethodContext InputMethodContext::FromWindowHandle(HWND window_handle) {
    ZAF_EXPECT(window_handle);
    auto context_handle = ImmGetContext(window_handle);
    return InputMethodContext{ window_handle, context_handle };
}


InputMethodContext::InputMethodContext(HWND window_handle, HIMC context_handle) :
    window_handle_(window_handle),
    context_handle_(context_handle) {

    ZAF_EXPECT(window_handle_);
    ZAF_EXPECT(context_handle_);
}


InputMethodContext::~InputMethodContext() {

    ImmReleaseContext(window_handle_, context_handle_);
}


void InputMethodContext::MoveCompositionWindow(const Point& position_in_window) {

    auto position = FromDIPs(
        position_in_window, 
        static_cast<float>(GetDpiForWindow(window_handle_)));

    COMPOSITIONFORM form{};
    form.dwStyle = CFS_POINT;
    form.ptCurrentPos = position.ToPOINT();
    ImmSetCompositionWindow(context_handle_, &form);
}


void InputMethodContext::SetCompositionFont(const Font& font) {

    auto logfont = font.ToLOGFONT();
    ImmSetCompositionFont(context_handle_, &logfont);
}

}