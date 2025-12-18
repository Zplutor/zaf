#include <zaf/window/internal/window_facets/window_render_facet.h>
#include <zaf/window/internal/window_facets/window_inspect_facet.h>
#include <zaf/window/internal/window_facets/window_lifecycle_facet.h>
#include <zaf/control/control.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/dpi.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/internal/theme.h>
#include <zaf/window/window.h>

namespace zaf::internal {

WindowRenderFacet::WindowRenderFacet(Window& window) noexcept : window_(window) {

}


void WindowRenderFacet::CreateRenderer() {

    window_.lifecycle_facet_->HandleStateData().renderer =
        GraphicFactory::Instance().CreateWindowRenderer(window_.Handle());
}


void WindowRenderFacet::HandleWMPAINT() {

    auto handle = window_.Handle();

    zaf::Rect dirty_rect;
    RECT win32_rect{};
    if (GetUpdateRect(handle, &win32_rect, TRUE)) {
        dirty_rect = ToDIPs(Rect::FromRECT(win32_rect), window_.DPI());
    }
    else {
        dirty_rect = window_.root_control_->Rect();
    }

    //The update rect must be validated before painting.
    //Because some controls may call NeedRepaint while it is painting,
    //and this may fails if there is an invalidated update rect.
    ValidateRect(handle, nullptr);

    auto renderer = window_.lifecycle_facet_->HandleStateData().renderer;
    renderer.BeginDraw();
    Canvas canvas(renderer);
    {
        auto layer_guard = canvas.PushRegion(window_.root_control_->Rect(), dirty_rect);

        //Paint window background color first.
        {
            auto state_guard = canvas.PushState();
            canvas.SetBrushWithColor(Color::FromRGB(internal::ControlBackgroundColorRGB));
            canvas.DrawRectangle(dirty_rect);
        }

        window_.root_control_->Repaint(canvas, dirty_rect);

        window_.inspect_facet_->PaintInspectedControl(canvas, dirty_rect);
    }

    try {
        renderer.EndDraw();
    }
    catch (const COMError& error) {
        if (error.code() == COMError::MakeCode(D2DERR_RECREATE_TARGET)) {
            RecreateRenderer();
        }
    }
}


void WindowRenderFacet::RecreateRenderer() {

    window_.root_control_->ReleaseRendererResources();
    CreateRenderer();
}

}