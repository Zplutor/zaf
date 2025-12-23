#include <zaf/window/internal/window_facets/window_inspect_facet.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/d2d/path_geometry.h>
#include <zaf/graphic/d2d/rectangle_geometry.h>
#include <zaf/internal/theme.h>
#include <zaf/window/inspector/inspector_window.h>
#include <zaf/window/internal/window_facets/window_lifecycle_facet.h>
#include <zaf/window/invalid_handle_state_error.h>
#include <zaf/window/window.h>

namespace zaf::internal {

WindowInspectFacet::WindowInspectFacet(Window& window) noexcept : window_(window) {

}


void WindowInspectFacet::ShowInspector() const {

    if (window_.HandleState() != WindowHandleState::Created) {
        throw InvalidHandleStateError{ ZAF_SOURCE_LOCATION() };
    }

    auto& inspector_window = window_.lifecycle_facet_->HandleStateData().inspector_window;
    if (inspector_window.lock()) {
        return;
    }

    auto new_inspector_window = Create<InspectorWindow>(window_.shared_from_this());
    new_inspector_window->Show();

    inspector_window = new_inspector_window;
}


void WindowInspectFacet::BeginSelectInspectedControl() {
    window_.lifecycle_facet_->HandleStateData().is_selecting_inspector_control = true;
}


bool WindowInspectFacet::IsSelectingInspectedControl() const noexcept {
    return window_.lifecycle_facet_->HandleStateData().is_selecting_inspector_control;
}


void WindowInspectFacet::SelectInspectedControl() {

    auto& handle_state_data = window_.lifecycle_facet_->HandleStateData();
    handle_state_data.is_selecting_inspector_control = false;

    if (!handle_state_data.highlight_control) {
        return;
    }

    auto inspector_port = GetInspectorPort();
    if (inspector_port) {
        inspector_port->SelectControl(handle_state_data.highlight_control);
    }

    SetHighlightControl(nullptr);
}


void WindowInspectFacet::SetHighlightControl(const std::shared_ptr<Control>& control) {
    
    auto& state_data = window_.lifecycle_facet_->HandleStateData();
    if (state_data.highlight_control == control) {
        return;
    }

    if (!control) {
        state_data.highlight_control = nullptr;
        window_.NeedRepaintRect(window_.RootControl()->Rect());
        return;
    }

    if (control->Window().get() != &window_) {
        return;
    }

    //Repaint the rect of previous highlight control.
    if (state_data.highlight_control) {
        auto rect_in_window = state_data.highlight_control->RectInWindow();
        if (rect_in_window) {
            window_.NeedRepaintRect(*rect_in_window);
        }
    }

    state_data.highlight_control = control;

    //Repaint the rect of new highlight control.
    window_.NeedRepaintRect(*state_data.highlight_control->RectInWindow());
}


void WindowInspectFacet::HighlightControlAtPosition(const Point& position) {

    auto highlight_control = window_.RootControl()->FindChildAtPositionRecursively(position);
    if (!highlight_control) {
        highlight_control = window_.RootControl();
    }

    SetHighlightControl(highlight_control);

    auto inspector_port = GetInspectorPort();
    if (inspector_port) {
        inspector_port->HighlightControl(highlight_control);
    }
}


std::shared_ptr<InspectorPort> WindowInspectFacet::GetInspectorPort() const noexcept {

    auto handle_state = window_.HandleState();
    if (handle_state != WindowHandleState::Created) {
        return nullptr;
    }

    auto& state_data = window_.lifecycle_facet_->HandleStateData();
    auto inspector_window = state_data.inspector_window.lock();
    if (!inspector_window) {
        return nullptr;
    }

    return inspector_window->GetPort();
}


void WindowInspectFacet::PaintInspectedControl(Canvas& canvas, const Rect& dirty_rect) {

    auto highlight_control = window_.lifecycle_facet_->HandleStateData().highlight_control;
    if (!highlight_control) {
        return;
    }

    auto control_rect = highlight_control->RectInWindow();
    if (!control_rect) {
        return;
    }

    if (!control_rect->HasIntersection(dirty_rect)) {
        return;
    }

    auto padding_rect = *control_rect;
    padding_rect.Deflate(highlight_control->Border());

    auto content_rect = padding_rect;
    content_rect.Deflate(highlight_control->Padding());

    auto margin_rect = *control_rect;
    margin_rect.Inflate(highlight_control->Margin());

    auto draw_frame = [&canvas](
        const zaf::Rect& rect,
        const zaf::Rect excluded_rect,
        std::uint32_t color_rgb) {

            auto rect_geometry = canvas.CreateRectangleGeometry(rect);
            auto excluded_geometry = canvas.CreateRectangleGeometry(excluded_rect);

            auto frame_geometry = canvas.CreatePathGeometry();
            auto sink = frame_geometry.Open();
            d2d::Geometry::Combine(
                rect_geometry,
                excluded_geometry,
                d2d::Geometry::CombineMode::Exclude,
                sink);
            sink.Close();

            auto color = Color::FromRGB(color_rgb);
            color.a /= 2.f;
            canvas.SetBrushWithColor(color);
            canvas.DrawGeometry(frame_geometry);
        };

    auto state_guard = canvas.PushState();
    auto clipping_guard = canvas.PushClipping(dirty_rect);

    //Draw content rect.
    draw_frame(content_rect, zaf::Rect{}, internal::InspectedControlContentColor);

    //Draw padding rect.
    draw_frame(padding_rect, content_rect, internal::InspectedControlPaddingColor);

    //Draw border rect.
    draw_frame(*control_rect, padding_rect, internal::InspectedControlBorderColor);

    //Draw margin rect.
    draw_frame(margin_rect, *control_rect, internal::InspectedControlMarginColor);
}

}