#include <zaf/graphic/d2d/bitmap_renderer.h>

namespace zaf::d2d {

BitmapRenderer::BitmapRenderer(COMPtr<ID2D1BitmapRenderTarget> ptr) : 
    Renderer(ptr), 
    ptr_(std::move(ptr)) {

}


RenderBitmap BitmapRenderer::GetBitmap() const {

    COMPtr<ID2D1Bitmap> inner;
    auto com_error = Ptr()->GetBitmap(inner.Reset());

    ZAF_THROW_IF_COM_ERROR(com_error);
    return RenderBitmap(inner);
}

}