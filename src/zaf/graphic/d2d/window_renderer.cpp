#include <zaf/graphic/d2d/window_renderer.h>

namespace zaf::d2d {

WindowRenderer::WindowRenderer(COMPtr<ID2D1HwndRenderTarget> ptr) : 
    Renderer(ptr), 
    ptr_(std::move(ptr)) {

}


void WindowRenderer::Resize(const Size& size) {
    HRESULT result = Ptr()->Resize(size.ToD2D1SIZEU());
    ZAF_THROW_IF_COM_ERROR(result);
}

}