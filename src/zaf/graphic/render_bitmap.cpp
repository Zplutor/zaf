#include <zaf/graphic/render_bitmap.h>
#include <zaf/graphic/renderer/renderer.h>

namespace zaf {

void RenderBitmap::CopyFromRenderer(const Renderer& renderer, const Rect& renderer_rect, const Point& to_position, std::error_code& error_code) {
    D2D1_POINT_2U d2d_to_position = to_position.ToD2D1POINT2U();
    D2D1_RECT_U d2d_renderer_rect = renderer_rect.ToD2D1RECTU();
    HRESULT result = GetHandle()->CopyFromRenderTarget(&d2d_to_position, renderer.GetHandle(), &d2d_renderer_rect);
    error_code = MakeComErrorCode(result);
}

}