#include <zaf/graphic/renderer/bitmap_renderer.h>

namespace zaf {

RenderBitmap BitmapRenderer::GetBitmap() const {

    ID2D1Bitmap* handle = nullptr;
    auto com_error = Inner()->GetBitmap(&handle);

    ZAF_THROW_IF_COM_ERROR(com_error);
    return RenderBitmap(handle);
}

}