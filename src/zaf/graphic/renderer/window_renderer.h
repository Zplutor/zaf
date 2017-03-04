#pragma once

#include <zaf/graphic/renderer/renderer.h>

namespace zaf {

class WindowRenderer : public Renderer {
public:
    WindowRenderer() { }
    explicit WindowRenderer(ID2D1HwndRenderTarget* handle) : Renderer(handle) { }

    void Resize(const Size& size, std::error_code& error_code) {
        HRESULT result = GetHandle()->Resize(size.ToD2D1SIZEU());
        error_code = MakeComErrorCode(result);
    }

    void Resize(const Size& size) {
        std::error_code error_code;
        Resize(size, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    ID2D1HwndRenderTarget* GetHandle() const {
        return static_cast<ID2D1HwndRenderTarget*>(__super::GetHandle());
    }
};

}