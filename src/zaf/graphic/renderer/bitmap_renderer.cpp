#include <zaf/graphic/renderer/bitmap_renderer.h>

namespace zaf {

RendererBitmap BitmapRenderer::GetBitmap(std::error_code& error_code) const {

    ID2D1Bitmap* handle = nullptr;
    auto com_error = GetHandle()->GetBitmap(&handle);

    error_code = MakeComErrorCode(com_error);
    return RendererBitmap(handle);
}

}