#include <zaf/graphic/d2d/render_bitmap.h>
#include <zaf/graphic/d2d/renderer.h>

namespace zaf::d2d {

void RenderBitmap::CopyFromRenderer(
    const d2d::Renderer& renderer, 
    const Rect& renderer_rect, 
    const Point& to_position) {

    D2D1_POINT_2U d2d_to_position = to_position.ToD2D1POINT2U();
    D2D1_RECT_U d2d_renderer_rect = renderer_rect.ToD2D1RECTU();
    HRESULT result = Ptr()->CopyFromRenderTarget(
        &d2d_to_position, 
        renderer.Ptr().Inner(),
        &d2d_renderer_rect);
    ZAF_THROW_IF_COM_ERROR(result);
}

}