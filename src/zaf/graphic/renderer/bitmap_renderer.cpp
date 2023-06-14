#include <zaf/graphic/renderer/bitmap_renderer.h>

namespace zaf {

RenderBitmap BitmapRenderer::GetBitmap() const {

    COMPtr<ID2D1Bitmap> inner;
    auto com_error = Inner()->GetBitmap(inner.Reset());

    ZAF_THROW_IF_COM_ERROR(com_error);
    return RenderBitmap(inner);
}

}