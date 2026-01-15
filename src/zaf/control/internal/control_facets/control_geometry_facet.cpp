#include <zaf/control/internal/control_facets/control_geometry_facet.h>
#include <algorithm>
#include <zaf/base/auto_reset.h>
#include <zaf/control/control.h>
#include <zaf/control/internal/control_updating.h>
#include <zaf/window/window.h>

#undef max
#undef min

namespace zaf::internal {

ControlGeometryFacet::ControlGeometryFacet(Control& control) noexcept : control_(control) {

}


const zaf::Rect& ControlGeometryFacet::Rect() const noexcept {
    return rect_;
}


void ControlGeometryFacet::SetRect(const zaf::Rect& rect) {

    zaf::Rect previous_rect = Rect();

    zaf::Rect new_rect{ rect.position, ApplySizeLimit(rect.size) };

    if (new_rect.size != previous_rect.size) {
        //Auto size.
        ApplyAutoSizeOnRectChanged(new_rect.size);
    }

    //Don't layout if rects are the same.
    if (new_rect == previous_rect) {
        return;
    }

    rect_ = new_rect;

    if (rect_.size != previous_rect.size) {

        control_.ReleaseCachedPaintingRenderer();

        //Layout children if size is changed.
        control_.NeedRelayout(previous_rect);
    }

    //The focused control need to be notified while its absolute position changed, 
    //so that it can relayout its elements, if needed.
    auto window = control_.Window();
    if (window) {
        auto focused_control = window->FocusedControl();
        if (focused_control) {
            if (control_.IsAncestorOf(*focused_control)) {
                focused_control->NeedRelayout();
            }
        }
    }

    //Notify rect change.
    RectChangedInfo event_info{ control_.shared_from_this(), previous_rect };
    control_.OnRectChanged(event_info);

    auto parent = control_.Parent();
    if (parent) {
        parent->OnChildRectChanged(control_.shared_from_this(), previous_rect);
    }
}


zaf::Rect ControlGeometryFacet::RectInSelf() const noexcept {
    return zaf::Rect{ zaf::Point{}, this->Size() };
}


std::optional<zaf::Rect> ControlGeometryFacet::RectInWindow() const noexcept {

    auto position_in_window = PositionInWindow();
    if (!position_in_window) {
        return std::nullopt;
    }

    return zaf::Rect{ *position_in_window, this->Size() };
}


const Point& ControlGeometryFacet::Position() const noexcept {
    return rect_.position;
}


void ControlGeometryFacet::SetPosition(const Point& position) {
    SetRect(zaf::Rect(position, Rect().size));
}


std::optional<zaf::Point> ControlGeometryFacet::PositionInWindow() const noexcept {

    auto parent = control_.Parent();

    //Current control has no parent, it may be the root control of a window.
    if (!parent) {

        auto window = control_.Window();
        if (window) {
            //Current control is the root control, whose position is the position in window.
            return this->Position();
        }

        //Current control doesn't belong to any window.
        return std::nullopt;
    }

    auto parent_position_in_window = parent->PositionInWindow();
    if (!parent_position_in_window) {
        return std::nullopt;
    }

    auto result = *parent_position_in_window;
    result.AddOffset(parent->ContentRect().position);
    result.AddOffset(this->Position());
    return result;
}


float ControlGeometryFacet::X() const noexcept {
    return Position().x;
}


void ControlGeometryFacet::SetX(float x) {
    SetPosition(Point(x, Y()));
}


float ControlGeometryFacet::Y() const noexcept {
    return Position().y;
}


void ControlGeometryFacet::SetY(float y) {
    SetPosition(Point(X(), y));
}


const zaf::Size& ControlGeometryFacet::Size() const noexcept {
    return rect_.size;
}


void ControlGeometryFacet::SetSize(const zaf::Size& size) {
    SetRect(zaf::Rect(Rect().position, size));
}


float ControlGeometryFacet::Width() const noexcept {
    return rect_.size.width;
}


void ControlGeometryFacet::SetWidth(float width) {
    SetSize(zaf::Size(width, Height()));
}


float ControlGeometryFacet::Height() const noexcept {
    return rect_.size.height;
}


void ControlGeometryFacet::SetHeight(float height) {
    SetSize(zaf::Size(Width(), height));
}


float ControlGeometryFacet::MinWidth() const noexcept {
    return min_width_;
}


void ControlGeometryFacet::SetMinWidth(float min_width) {

    min_width_ = min_width;

    if (MaxWidth() < min_width) {
        SetMaxWidth(min_width);
    }

    if (Width() < min_width) {
        SetWidth(min_width);
    }
}


float ControlGeometryFacet::MaxWidth() const noexcept {
    return max_width_;
}


void ControlGeometryFacet::SetMaxWidth(float max_width) {

    max_width_ = max_width;

    if (MinWidth() > max_width) {
        SetMinWidth(max_width);
    }

    if (Width() > max_width) {
        SetWidth(max_width);
    }
}


void ControlGeometryFacet::SetFixedWidth(float width) {
    SetFixedWidthValue(width);
    SetWidth(width);
}


float ControlGeometryFacet::MinHeight() const noexcept {
    return min_height_;
}


void ControlGeometryFacet::SetMinHeight(float min_height) {

    min_height_ = min_height;

    if (MaxHeight() < min_height) {
        SetMaxHeight(min_height);
    }

    if (Height() < min_height) {
        SetHeight(min_height);
    }
}


float ControlGeometryFacet::MaxHeight() const noexcept {
    return max_height_;
}


void ControlGeometryFacet::SetMaxHeight(float max_height) {

    max_height_ = max_height;

    if (MinHeight() > max_height) {
        SetMinHeight(max_height);
    }

    if (Height() > max_height) {
        SetHeight(max_height);
    }
}


void ControlGeometryFacet::SetFixedHeight(float height) {
    SetFixedHeightValue(height);
    SetHeight(height);
}


void ControlGeometryFacet::SetFixedSize(const zaf::Size& size) {
    SetFixedWidthValue(size.width);
    SetFixedHeightValue(size.height);
    SetSize(size);
}


zaf::Size ControlGeometryFacet::CalculatePreferredSize() const {

    zaf::Size bound_size{ 
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max() 
    };

    return CalculatePreferredSize(bound_size);
}


zaf::Size ControlGeometryFacet::CalculatePreferredSize(const zaf::Size& bound_size) const {

    float non_content_width{};
    float non_content_height{};

    const auto& padding = Padding();
    non_content_width += padding.left + padding.right;
    non_content_height += padding.top + padding.bottom;

    const auto& border = Border();
    non_content_width += border.left + border.right;
    non_content_height += border.top + border.bottom;

    zaf::Size max_content_size{
        std::max(bound_size.width - non_content_width, 0.f),
        std::max(bound_size.height - non_content_height, 0.f)
    };
    auto preferred_content_size = control_.CalculatePreferredContentSize(max_content_size);

    zaf::Size result{
        preferred_content_size.width + non_content_width,
        preferred_content_size.height + non_content_height
    };
    return result;
}


bool ControlGeometryFacet::AutoWidth() const {
    return auto_width_;
}


void ControlGeometryFacet::SetAutoWidth(bool value) {
    auto_width_ = value;
    AutoResizeToPreferredSize();
}


bool ControlGeometryFacet::AutoHeight() const {
    return auto_height_;
}


void ControlGeometryFacet::SetAutoHeight(bool value) {
    auto_height_ = value;
    AutoResizeToPreferredSize();
}


void ControlGeometryFacet::SetAutoSize(bool value) {

    auto update_guard = control_.BeginUpdate();

    SetAutoWidth(true);
    SetAutoHeight(true);
}


float ControlGeometryFacet::ApplyWidthLimit(float width) const {

    float result = width;
    result = std::max(result, MinWidth());
    result = std::min(result, MaxWidth());
    return result;
}


float ControlGeometryFacet::ApplyHeightLimit(float height) const {

    float result = height;
    result = std::max(result, MinHeight());
    result = std::min(result, MaxHeight());
    return result;
}


zaf::Size ControlGeometryFacet::ApplySizeLimit(const zaf::Size& size) const {

    return zaf::Size{ 
        ApplyWidthLimit(size.width),
        ApplyHeightLimit(size.height) 
    };
}


zaf::Anchor ControlGeometryFacet::Anchor() const {
    return anchor_;
}


void ControlGeometryFacet::SetAnchor(zaf::Anchor anchor) {
    anchor_ = anchor;
}


const Frame& ControlGeometryFacet::Margin() const {
    return margin_;
}


void ControlGeometryFacet::SetMargin(const Frame& margin) {

    margin_ = margin;

    //Notify parent to re-layout all children.
    auto parent = control_.Parent();
    if (parent) {

        parent->NeedRelayout();
        parent->AutoResizeToPreferredSize();
    }
}


const Frame& ControlGeometryFacet::Border() const {
    return border_;
}


void ControlGeometryFacet::SetBorder(const Frame& border) {
    border_ = border;
    control_.NeedRepaint();
    control_.NeedRelayout();
}


const Frame& ControlGeometryFacet::Padding() const {
    return padding_;
}


void ControlGeometryFacet::SetPadding(const Frame& padding) {
    padding_ = padding;
    control_.NeedRelayout();
}


std::optional<zaf::Rect> ControlGeometryFacet::ContentRectInWindow() const noexcept {

    auto rect_in_window = this->RectInWindow();
    if (!rect_in_window) {
        return std::nullopt;
    }

    auto result = ContentRect();
    result.AddOffset(rect_in_window->position);
    return result;
}


zaf::Rect ControlGeometryFacet::ContentRect() const {

    zaf::Rect content_rect = zaf::Rect(Point(), Size());
    content_rect.Deflate(Border());
    content_rect.Deflate(Padding());

    //Make sure the width and the height are not less than 0.
    content_rect.size.width = (std::max)(content_rect.size.width, 0.f);
    content_rect.size.height = (std::max)(content_rect.size.height, 0.f);

    return content_rect;
}


zaf::Size ControlGeometryFacet::ContentSize() const {
    return ContentRect().size;
}


void ControlGeometryFacet::SetFixedWidthValue(float value) {
    min_width_ = value;
    max_width_ = value;
}


void ControlGeometryFacet::SetFixedHeightValue(float value) {
    min_height_ = value;
    max_height_ = value;
}


void ControlGeometryFacet::ApplyAutoSizeOnRectChanged(zaf::Size& new_size) {

    if (control_.is_auto_resizing_) {
        return;
    }

    if (!auto_width_ && !auto_height_) {
        return;
    }

    auto preferred_size = CalculatePreferredSizeForAutoSize(new_size);

    if (auto_width_) {
        SetFixedWidthValue(preferred_size.width);
        new_size.width = preferred_size.width;
    }

    if (auto_height_) {
        SetFixedHeightValue(preferred_size.height);
        new_size.height = preferred_size.height;
    }
}


void ControlGeometryFacet::AutoResizeToPreferredSize() {

    if (!auto_width_ && !auto_height_) {
        return;
    }

    if (control_.update_state_) {
        control_.update_state_->need_resize = true;
        return;
    }

    auto preferred_size = CalculatePreferredSizeForAutoSize(Size());

    auto update_guard = control_.BeginUpdate();
    auto auto_reset = MakeAutoReset(control_.is_auto_resizing_, true);

    if (auto_width_) {
        SetFixedWidth(preferred_size.width);
    }

    if (auto_height_) {
        SetFixedHeight(preferred_size.height);
    }
}


zaf::Size ControlGeometryFacet::CalculatePreferredSizeForAutoSize(const zaf::Size& control_size) const {

    zaf::Size bound_size;
    bound_size.width = auto_width_ ? std::numeric_limits<float>::max() : control_size.width;
    bound_size.height = auto_height_ ? std::numeric_limits<float>::max() : control_size.height;

    return CalculatePreferredSize(bound_size);
}

}
