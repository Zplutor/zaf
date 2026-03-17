#include <zaf/window/input_method_context.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/base/error/unknown_runtime_error.h>
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


std::wstring InputMethodContext::GetCompositionString() const {
    return InnerGetCompositionString(GCS_COMPSTR);
}


std::wstring InputMethodContext::GetResultString() const {
    return InnerGetCompositionString(GCS_RESULTSTR);
}


std::wstring InputMethodContext::InnerGetCompositionString(DWORD type) const {

    LONG size = ImmGetCompositionString(context_handle_, type, nullptr, 0);
    if (size == 0) {
        return {};
    }

    if (size < 0) {
        throw UnknownRuntimeError(ZAF_SOURCE_LOCATION());
    }

    std::wstring result;
    result.resize(size / sizeof(wchar_t));

    size = ImmGetCompositionStringW(context_handle_, type, result.data(), size);
    if (size < 0) {
        throw UnknownRuntimeError(ZAF_SOURCE_LOCATION());
    }

    result.resize(size / sizeof(wchar_t));
    return result;
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