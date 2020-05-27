#pragma once

#include <zaf/graphic/renderer/renderer.h>

namespace zaf {

class WindowRenderer : public Renderer {
public:
    WindowRenderer() { }
    explicit WindowRenderer(ID2D1HwndRenderTarget* handle) : Renderer(handle) { }

    void Resize(const Size& size) {
        HRESULT result = GetHandle()->Resize(size.ToD2D1SIZEU());
        ZAF_THROW_IF_COM_ERROR(result);
    }

    ID2D1HwndRenderTarget* GetHandle() const {
        return static_cast<ID2D1HwndRenderTarget*>(__super::GetHandle());
    }
};

}