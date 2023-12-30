#include <zaf/control/control.h>
#include <algorithm>
#include <zaf/application.h>
#include <zaf/base/as.h>
#include <zaf/base/auto_reset.h>
#include <zaf/base/define.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/base/error/check.h>
#include <zaf/control/control_parser.h>
#include <zaf/control/internal/cached_painting.h>
#include <zaf/control/internal/control_updating.h>
#include <zaf/control/internal/image_box/image_drawing.h>
#include <zaf/control/layout/anchor_layouter.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/geometry/rectangle_geometry.h>
#include <zaf/graphic/image/image.h>
#include <zaf/internal/theme.h>
#include <zaf/object/type_definition.h>
#include <zaf/rx/subject.h>
#include <zaf/window/inspector/internal/inspector_port.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>
#include <zaf/window/window.h>

#undef max
#undef min

namespace zaf {
namespace {

float GetDPIFromWindow(const std::shared_ptr<Window>& window) {
    if (window) {
        return window->GetDPI();
    }
    return Application::Instance().GetSystemDPI();
}

constexpr bool DefaultCanFocused = false;
constexpr bool DefaultIsEnabled = true;
constexpr bool DefaultIsVisible = true;

}


ZAF_DEFINE_TYPE(Control)
ZAF_DEFINE_TYPE_PARSER(ControlParser)
ZAF_DEFINE_TYPE_PROPERTY(Rect)
ZAF_DEFINE_TYPE_PROPERTY(Position)
ZAF_DEFINE_TYPE_PROPERTY(X)
ZAF_DEFINE_TYPE_PROPERTY(Y)
ZAF_DEFINE_TYPE_PROPERTY(Size)
ZAF_DEFINE_TYPE_PROPERTY(Width)
ZAF_DEFINE_TYPE_PROPERTY(MinWidth)
ZAF_DEFINE_TYPE_PROPERTY(MaxWidth)
ZAF_DEFINE_TYPE_PROPERTY(FixedWidth)
ZAF_DEFINE_TYPE_PROPERTY(FixedHeight)
ZAF_DEFINE_TYPE_PROPERTY(FixedSize)
ZAF_DEFINE_TYPE_PROPERTY(Height)
ZAF_DEFINE_TYPE_PROPERTY(MinHeight)
ZAF_DEFINE_TYPE_PROPERTY(MaxHeight)
ZAF_DEFINE_TYPE_PROPERTY(AutoWidth)
ZAF_DEFINE_TYPE_PROPERTY(AutoHeight)
ZAF_DEFINE_TYPE_PROPERTY(AutoSize)
ZAF_DEFINE_TYPE_PROPERTY(ContentRect)
ZAF_DEFINE_TYPE_PROPERTY(ContentSize)
ZAF_DEFINE_TYPE_PROPERTY(Name)
ZAF_DEFINE_TYPE_PROPERTY(IsVisible)
ZAF_DEFINE_TYPE_PROPERTY(IsEnabled)
ZAF_DEFINE_TYPE_PROPERTY(CanFocused)
ZAF_DEFINE_TYPE_PROPERTY(CanTabStop)
ZAF_DEFINE_TYPE_PROPERTY(Margin)
ZAF_DEFINE_TYPE_PROPERTY(Border)
ZAF_DEFINE_TYPE_PROPERTY(Padding)
ZAF_DEFINE_TYPE_PROPERTY(BackgroundColor)
ZAF_DEFINE_TYPE_PROPERTY(BorderColor)
ZAF_DEFINE_TYPE_PROPERTY(BackgroundImageLayout)
ZAF_DEFINE_TYPE_PROPERTY(BackgroundImage)
ZAF_DEFINE_TYPE_PROPERTY(Parent)
ZAF_DEFINE_TYPE_PROPERTY(Window)
ZAF_DEFINE_TYPE_PROPERTY(CanDoubleClick)
ZAF_DEFINE_TYPE_PROPERTY_DYNAMIC(Layouter)
ZAF_DEFINE_TYPE_PROPERTY(Tooltip)
ZAF_DEFINE_TYPE_END


Control::Control() : 
    is_mouse_over_(false), 
    is_capturing_mouse_(false),
    is_focused_(false),
    can_focused_(DefaultCanFocused),
    is_enabled_(DefaultIsEnabled),
    is_visible_(DefaultIsVisible) {

}


Control::~Control() {

}


void Control::InvokeInitialize() {

    //Enter update state during initialization.
    auto update_guard = BeginUpdate();

    __super::InvokeInitialize();
}


void Control::InvokeParse() {

    //Enter update state during parsing.
    auto update_guard = BeginUpdate();

    __super::InvokeParse();
}


ControlUpdateGuard Control::BeginUpdate() {

    auto lock = update_lock_.lock();
    if (!lock) {

        lock = std::make_shared<internal::ControlUpdateLock>(*this);
        update_lock_ = lock;

        ZAF_EXPECT(!update_state_);
        update_state_ = std::make_unique<internal::ControlUpdateState>();
    }

    return ControlUpdateGuard{ lock };
}


void Control::EndUpdate() {

    ZAF_EXPECT(update_state_);
    auto update_state = std::move(update_state_);

    //BeginUpdate may be called again while control is updating in EndUpdate,
    //thus the lock must be reset to avoid abnormal behaviour.
    update_lock_.reset();

    if (update_state->need_relayout) {
        NeedRelayout();
    }

    if (!update_state->need_repainted_rect.IsEmpty()) {
        NeedRepaintRect(update_state->need_repainted_rect);
    }

    if (update_state->need_resize) {
        AutoResizeToPreferredSize();
    }
}


void Control::Repaint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    //Make sure the control repaints only if it is visible.
    ZAF_EXPECT(IsVisible());

    if (IsCachedPaintingEnabled()) {
        RepaintUsingCachedPainting(canvas, dirty_rect);
    }
    else {
        RepaintControl(canvas, dirty_rect, false);
    }
}


void Control::RepaintUsingCachedPainting(Canvas& canvas, const zaf::Rect& dirty_rect) {

    auto control_size = Size();
    control_size.width = std::ceil(control_size.width);
    control_size.height = std::ceil(control_size.height);

    //Create the cached renderer if it is not created.
    if (!cached_renderer_) {
        CreateCompatibleRendererOptions options;
        options.DesiredSize(control_size);
        cached_renderer_ = canvas.Renderer().CreateCompatibleRenderer(options);
        valid_cached_renderer_rect_ = zaf::Rect{};
    }

    //Calculate the actual dirty rect that needs to repaint, and the new valid rect in cached 
    //renderer.
    auto calculate_result = internal::CalculateDirtyRectOutsideValidCachedRect(
        dirty_rect, 
        valid_cached_renderer_rect_);

    //Repaint the actual dirty rect is it is not empty.
    if (!calculate_result.actual_dirty_rect.IsEmpty()) {

        cached_renderer_.BeginDraw();
        Canvas cached_painting_canvas(cached_renderer_);
        {
            auto layer_guard = cached_painting_canvas.PushRegion(
                zaf::Rect{ Point{}, control_size },
                calculate_result.actual_dirty_rect);

            RepaintControl(cached_painting_canvas, calculate_result.actual_dirty_rect, true);
        }
        cached_renderer_.EndDraw();

        //Update valid cached rect after repainting.
        valid_cached_renderer_rect_ = calculate_result.new_valid_cached_rect;
    }

    //Paint into canvas from cached renderer.
    canvas.DrawBitmap(
        cached_renderer_.GetBitmap(),
        dirty_rect, 
        DrawImageOptions().SourceRect(dirty_rect));
}


void Control::RepaintControl(Canvas& canvas, const zaf::Rect& dirty_rect, bool need_clear) {

    if (need_clear) {
        canvas.Clear();
    }
    Paint(canvas, dirty_rect);

    RepaintChildren(canvas, dirty_rect);
}


void Control::RepaintChildren(Canvas& canvas, const zaf::Rect& dirty_rect) {

    //No need to repaint if there is no child.
    if (children_.empty()) {
        return;
    }

    //No need to repaint if the content rect is not dirty.
    zaf::Rect content_rect = ContentRect();
    if (!content_rect.HasIntersection(dirty_rect)) {
        return;
    }

    for (const auto& child : children_) {

        //Don't paint the child if it is not visible.
        if (!child->IsVisible()) {
            continue;
        }

        zaf::Rect child_rect = child->Rect();
        child_rect.position.AddOffset(content_rect.position);

        //No need to paint the child if its rect not dirty.
        zaf::Rect child_dirty_rect = zaf::Rect::Intersect(child_rect, dirty_rect);
        if (child_dirty_rect.IsEmpty()) {
            continue;
        }

        child_dirty_rect.Intersect(content_rect);
        if (child_dirty_rect.IsEmpty()) {
            continue;
        }

        auto layer_guard = canvas.PushRegion(child_rect, child_dirty_rect);
        child_dirty_rect.position.SubtractOffset(child_rect.position);
        child->Repaint(canvas, child_dirty_rect);
    }
}


void Control::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    auto state_guard = canvas.PushState();

    zaf::Rect control_rect{ Point{}, Size() };

    //Draw background color.
    canvas.SetBrushWithColor(BackgroundColor());
    canvas.DrawRectangle(control_rect);

    //Draw background image.
    DrawBackgroundImage(canvas, control_rect);

    //Calculate border geometry and draw border.
    zaf::Rect inner_rect = control_rect;
    inner_rect.Deflate(Border());

    //The with and height must be greater than 0.
    if (inner_rect.size.width < 0) {
        inner_rect.size.width = 0;
    }
    if (inner_rect.size.height < 0) {
        inner_rect.size.height = 0;
    }

    auto control_geometry = canvas.CreateRectangleGeometry(control_rect);
    auto inner_geometry = canvas.CreateRectangleGeometry(inner_rect);
    
    auto border_geometry = canvas.CreatePathGeometry();
    auto sink = border_geometry.Open();
    Geometry::Combine(control_geometry, inner_geometry, Geometry::CombineMode::Exclude, sink);
    sink.Close();

    canvas.SetBrushWithColor(BorderColor());
    canvas.DrawGeometry(border_geometry);
}


void Control::DrawBackgroundImage(Canvas& canvas, const zaf::Rect& background_rect) {

    auto background_image = BackgroundImage();
    if (!background_image) {
        return;
    }
    
    auto render_bitmap = background_image->CreateRenderBitmap(canvas.Renderer());
    if (!render_bitmap) {
        return;
    }

    internal::DrawImage(
        canvas,
        background_rect,
        BackgroundImageLayout(),
        render_bitmap,
        InterpolationMode::Linear);
}


void Control::NeedRepaint() {
    NeedRepaintRect(zaf::Rect(Point(), rect_.size));
}


void Control::NeedRepaintRect(const zaf::Rect& rect) {

    if ((rect.size.width == 0) || (rect.size.height == 0)) {
        return;
    }

    if (update_state_) {
        update_state_->need_repainted_rect.Union(rect);
        return;
    }

    zaf::Rect bound_rect(Point(), rect_.size);
    zaf::Rect repaint_rect = zaf::Rect::Intersect(bound_rect, rect);
    if (repaint_rect.IsEmpty()) {
        return;
    }

    RecalculateCachedPaintingRect(repaint_rect);

    auto window = window_.lock();
    if (window != nullptr) {
        window->NeedRepaintRect(repaint_rect);
        return;
    }

    auto parent = Parent();
    if (parent == nullptr) {
        return;
    }

    Point position_in_parent = TranslatePositionToParent(repaint_rect.position);

    zaf::Rect repaint_rect_in_parent(position_in_parent, repaint_rect.size);
    repaint_rect_in_parent.Intersect(parent->ContentRect());
    parent->NeedRepaintRect(repaint_rect_in_parent);
}


void Control::RecalculateCachedPaintingRect(const zaf::Rect& repaint_rect) {

    if (!cached_renderer_) {
        return;
    }

    zaf::Rect invalid_rect = internal::CalculateInvalidRectInCachedRect(
        valid_cached_renderer_rect_,
        repaint_rect);

    if (invalid_rect.IsEmpty()) {
        return;
    }

    valid_cached_renderer_rect_.Subtract(invalid_rect);
}


void Control::ReleaseRendererResources() {

    ReleaseCachedPaintingRenderer();

    for (const auto& each_child : children_) {
        each_child->ReleaseRendererResources();
    }
}


void Control::OnChildRectChanged(
    const std::shared_ptr<Control>& child,
    const zaf::Rect& previous_rect) {

    const zaf::Rect& new_rect = child->Rect();

    if (new_rect.HasIntersection(previous_rect)) {
        NeedRepaintRect(zaf::Rect::Union(new_rect, previous_rect));
    }
    else {
        NeedRepaintRect(new_rect);
        NeedRepaintRect(previous_rect);
    }

    if (!is_layouting_ || (auto_width_ || auto_height_)) {
        NeedRelayout();
    }
    AutoResizeToPreferredSize();
}


void Control::Layout(const zaf::Rect& previous_rect) {

    auto layouter = Layouter();
    if (layouter) {
        layouter->Layout(*this, previous_rect, Children());
    }
}


void Control::NeedRelayout() {
    NeedRelayout(Rect());
}


void Control::NeedRelayout(const zaf::Rect& previous_rect) {

    if (update_state_) {
        update_state_->need_relayout = true;
    }
    else {

        //Avoid auto resize when layouting children.
        auto update_guard = BeginUpdate();

        auto auto_reset = MakeAutoReset(is_layouting_, true);
        Layout(previous_rect);
    }
}


std::optional<zaf::Point> Control::PositionInWindow() const noexcept {

    auto parent = Parent();

    //Current control has no parent, it may be the root control of a window.
    if (!parent) {

        auto window = Window();
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


std::optional<zaf::Rect> Control::RectInWindow() const noexcept {

    auto position_in_window = PositionInWindow();
    if (!position_in_window) {
        return std::nullopt;
    }

    return zaf::Rect{ *position_in_window, this->Size() };
}


zaf::Rect Control::RectInSelf() const noexcept {
    return zaf::Rect{ zaf::Point{}, this->Size() };
}


void Control::SetRect(const zaf::Rect& rect) {

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

        ReleaseCachedPaintingRenderer();

        //Layout children if size is changed.
        NeedRelayout(previous_rect);
    }

    //The focused control need to be notified while its absolute position changed, 
    //so that it can relayout its elements, if needed.
    auto window = Window();
    if (window) {
        auto focused_control = window->FocusedControl();
        if (focused_control) {
            if (IsAncestorOf(*focused_control)) {
                focused_control->NeedRelayout();
            }
        }
    }

    //Notify rect change.
    RectChangedInfo event_info{ shared_from_this(), previous_rect };
    OnRectChanged(event_info);

    auto parent = Parent();
    if (parent) {
        parent->OnChildRectChanged(shared_from_this(), previous_rect);
    }
}


void Control::OnRectChanged(const RectChangedInfo& event_info) {

    rect_changed_event_.Raise(event_info);

    if (this->Rect().position != event_info.PreviousRect().position) {

        OnPositionChanged(PositionChangedInfo{ 
            shared_from_this(),
            event_info.PreviousRect().position 
        });
    }

    if (this->Rect().size != event_info.PreviousRect().size) {

        OnSizeChanged(SizeChangedInfo{ 
            shared_from_this(), 
            event_info.PreviousRect().size
        });
    }
}


Observable<RectChangedInfo> Control::RectChangedEvent() const {
    return rect_changed_event_.GetObservable();
}


void Control::OnPositionChanged(const PositionChangedInfo& event_info) {
    position_changed_event_.Raise(event_info);
}


Observable<PositionChangedInfo> Control::PositionChangedEvent() const {
    return position_changed_event_.GetObservable();
}


void Control::OnSizeChanged(const SizeChangedInfo& event_info) {
    size_changed_event_.Raise(event_info);
}


Observable<SizeChangedInfo> Control::SizeChangedEvent() const {
    return size_changed_event_.GetObservable();
}


void Control::SetMargin(const Frame& margin) {

    margin_ = margin;

    //Notify parent to re-layout all children.
    auto parent = Parent();
    if (parent) {

        parent->NeedRelayout();
        parent->AutoResizeToPreferredSize();
    }
}


float Control::MinWidth() const {
    return min_width_;
}

void Control::SetMinWidth(float min_width) {

    min_width_ = min_width;

    if (MaxWidth() < min_width) {
        SetMaxWidth(min_width);
    }

    if (Width() < min_width) {
        SetWidth(min_width);
    }
}


float Control::MaxWidth() const {
    return max_width_;
}

void Control::SetMaxWidth(float max_width) {

    max_width_ = max_width;

    if (MinWidth() > max_width) {
        SetMinWidth(max_width);
    }

    if (Width() > max_width) {
        SetWidth(max_width);
    }
}


float Control::MinHeight() const {
    return min_height_;
}

void Control::SetMinHeight(float min_height) {

    min_height_ = min_height;

    if (MaxHeight() < min_height) {
        SetMaxHeight(min_height);
    }

    if (Height() < min_height) {
        SetHeight(min_height);
    }
}


float Control::MaxHeight() const {
    return max_height_;
}

void Control::SetMaxHeight(float max_height) {

    max_height_ = max_height;

    if (MinHeight() > max_height) {
        SetMinHeight(max_height);
    }

    if (Height() > max_height) {
        SetHeight(max_height);
    }
}


zaf::Size Control::CalculatePreferredSize() const {

    zaf::Size bound_size{ 
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max() 
    };

    return CalculatePreferredSize(bound_size);
}


zaf::Size Control::CalculatePreferredSize(const zaf::Size& bound_size) const {

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
    auto preferred_content_size = CalculatePreferredContentSize(max_content_size);

    zaf::Size result{
        preferred_content_size.width + non_content_width,
        preferred_content_size.height + non_content_height
    };
    return result;
}


zaf::Size Control::CalculatePreferredContentSize(const zaf::Size& bound_size) const {

    zaf::Rect union_rect;

    for (const auto& each_child : Children()) {

        if (!each_child->IsVisible()) {
            continue;
        }

        auto child_rect = each_child->Rect();
        child_rect.Inflate(each_child->Margin());

        zaf::Rect needed_rect;
        needed_rect.size.width = std::max(child_rect.position.x + child_rect.size.width, 0.f);
        needed_rect.size.height = std::max(child_rect.position.y + child_rect.size.height, 0.f);

        union_rect.Union(needed_rect);
    }

    zaf::Size result;
    result.width = std::min(union_rect.size.width, bound_size.width);
    result.height = std::min(union_rect.size.height, bound_size.height);
    return result;
}


void Control::ApplyAutoSizeOnRectChanged(zaf::Size& new_size) {

    if (is_auto_resizing_) {
        return;
    }

    if (!auto_width_ && !auto_height_) {
        return;
    }

    auto preferred_size = CalculatePreferredSizeForAutoSize(new_size);

    if (auto_width_) {
        new_size.width = preferred_size.width;
    }

    if (auto_height_) {
        new_size.height = preferred_size.height;
    }
}


void Control::AutoResizeToPreferredSize() {

    if (!auto_width_ && !auto_height_) {
        return;
    }

    if (update_state_) {
        update_state_->need_resize = true;
        return;
    }

    auto preferred_size = CalculatePreferredSizeForAutoSize(Size());

    auto update_guard = BeginUpdate();
    auto auto_reset = MakeAutoReset(is_auto_resizing_, true);

    if (auto_width_) {
        SetFixedWidth(preferred_size.width);
    }

    if (auto_height_) {
        SetFixedHeight(preferred_size.height);
    }
}


zaf::Size Control::CalculatePreferredSizeForAutoSize(const zaf::Size& control_size) const {

    zaf::Size bound_size;
    bound_size.width = auto_width_ ? std::numeric_limits<float>::max() : control_size.width;
    bound_size.height = auto_height_ ? std::numeric_limits<float>::max() : control_size.height;

    return CalculatePreferredSize(bound_size);
}


bool Control::AutoWidth() const {
    return auto_width_;
}

void Control::SetAutoWidth(bool value) {
    auto_width_ = value;
    AutoResizeToPreferredSize();
}


bool Control::AutoHeight() const {
    return auto_height_;
}

void Control::SetAutoHeight(bool value) {
    auto_height_ = value;
    AutoResizeToPreferredSize();
}


void Control::SetAutoSize(bool value) {

    auto update_guard = BeginUpdate();

    SetAutoWidth(true);
    SetAutoHeight(true);
}


float Control::ApplyWidthLimit(float width) const {

    float result = width;
    result = std::max(result, MinWidth());
    result = std::min(result, MaxWidth());
    return result;
}


float Control::ApplyHeightLimit(float height) const {

    float result = height;
    result = std::max(result, MinHeight());
    result = std::min(result, MaxHeight());
    return result;
}


zaf::Size Control::ApplySizeLimit(const zaf::Size& size) const {

    return zaf::Size{ 
        ApplyWidthLimit(size.width),
        ApplyHeightLimit(size.height) 
    };
}


void Control::RaiseContentChangedEvent() {
    AutoResizeToPreferredSize();
}


Anchor Control::Anchor() const {
    return anchor_;
}


void Control::SetAnchor(zaf::Anchor anchor) {
    anchor_ = anchor;
}


std::optional<zaf::Rect> Control::ContentRectInWindow() const noexcept {

    auto rect_in_window = this->RectInWindow();
    if (!rect_in_window) {
        return std::nullopt;
    }

    auto result = ContentRect();
    result.AddOffset(rect_in_window->position);
    return result;
}


zaf::Rect Control::ContentRect() const {

    zaf::Rect content_rect = zaf::Rect(Point(), Size());
    content_rect.Deflate(Border());
    content_rect.Deflate(Padding());

    //Make sure the width and the height are not less than 0.
    content_rect.size.width = (std::max)(content_rect.size.width, 0.f);
    content_rect.size.height = (std::max)(content_rect.size.height, 0.f);

    return content_rect;
}


ImagePicker Control::BackgroundImagePicker() const {

    if (background_image_picker_) {
        return background_image_picker_;
    }
    return CreateImagePicker(nullptr);
}


void Control::SetBackgroundImagePicker(const ImagePicker& image_picker) {
    background_image_picker_ = image_picker;
    NeedRepaint();
}


ImageLayout Control::BackgroundImageLayout() const {
    return background_image_layout_;
}

void Control::SetBackgroundImageLayout(ImageLayout image_layout) {
    background_image_layout_ = image_layout;
    NeedRepaint();
}


ColorPicker Control::BackgroundColorPicker() const {

    if (background_color_picker_) {
        return background_color_picker_;
    }

    return [](const Control&) {
        return Color::Transparent();
    };
}


void Control::SetBackgroundColorPicker(const ColorPicker& color_picker) {

    background_color_picker_ = color_picker;
    NeedRepaint();
}


ColorPicker Control::BorderColorPicker() const {

    if (border_color_picker_) {
        return border_color_picker_;
    }

    return [](const Control&) {
        return Color::FromRGB(internal::ControlBackgroundColorRGB);
    };
}


void Control::SetBorderColorPicker(const ColorPicker& color_picker) {

    border_color_picker_ = color_picker;
    NeedRepaint();
}


std::shared_ptr<Layouter> Control::Layouter() const {

    if (layouter_) {
        return layouter_;
    }
    return GetAnchorLayouter();
}

void Control::SetLayouter(const std::shared_ptr<zaf::Layouter>& layouter) {
    layouter_ = layouter;
    NeedRelayout();
}


void Control::SetParent(const std::shared_ptr<Control>& parent) {

    auto previous_parent = parent_.lock();
    if (previous_parent == parent) {
        return;
    }

    std::shared_ptr<zaf::Window> previous_window;
    if (previous_parent) {
        previous_window = previous_parent->Window();
    }

    std::shared_ptr<zaf::Window> new_window;
    if (parent) {
        new_window = parent->Window();
    }

    bool need_release_renderer_resources = [&previous_window, &new_window]() {

        if (!previous_window) {
            return false;
        }

        if (!new_window) {
            return true;
        }

        return previous_window != new_window;
    }();

    if (need_release_renderer_resources) {
        ReleaseRendererResources();
    }

    parent_ = parent;

    ParentChangedInfo parent_changed_info{ shared_from_this(), previous_parent };
    OnParentChanged(parent_changed_info);

    if (previous_window != new_window) {
        RaiseWindowChangedEvent(previous_window);
    }

    if (GetDPIFromWindow(previous_window) != GetDPIFromWindow(new_window)) {
        RaiseDPIChangedEvent();
    }
}


void Control::SetChildren(const std::vector<std::shared_ptr<Control>>& children) {

    auto update_guard = BeginUpdate();
    RemoveAllChildren();
    AddChildren(children);
}


void Control::AddChild(const std::shared_ptr<Control>& child) {

    ZAF_EXPECT(child.get() != this);

    auto previous_parent = child->Parent();
    if (previous_parent.get() == this) {
        //Already added
        return;
    }

    if (previous_parent != nullptr) {
        //Remove child from previous parent
        previous_parent->InnerRemoveChild(child, false);
    }

    children_.push_back(child);
    child->SetParent(shared_from_this());

    NeedRelayout();
    NeedRepaintRect(child->Rect());

    AutoResizeToPreferredSize();

    auto inspector_port = GetInspectorPort();
    if (inspector_port) {
        inspector_port->ControlAddChild(shared_from_this());
    }
}


void Control::AddChildren(const std::vector<std::shared_ptr<Control>>& children) {

    auto update_guard = BeginUpdate();

    for (const auto& each_child : children) {
        AddChild(each_child);
    }
}


void Control::RemoveChildAtIndex(std::size_t index) {

    ZAF_EXPECT(index < children_.size());

    //We must use an local variable to maintain the life of the child for it will be accessed after 
    //deleted during remove.
    auto child = children_[index];
    RemoveChild(child);
}


void Control::RemoveChild(const std::shared_ptr<Control>& child) {
    InnerRemoveChild(child, true);
}


void Control::InnerRemoveChild(const std::shared_ptr<Control>& child, bool set_parent_to_null) {

    auto current_parent = child->Parent();
    if (current_parent.get() != this) {
        //Not child
        return;
    }

    //Remove focus before removing child.
    auto window = Window();
    if (window) {
        auto focused_control = window->FocusedControl();
        if (focused_control) {
            if (child->IsSameOrAncestorOf(*focused_control)) {
                focused_control->SetIsFocused(false);
            }
        }
    }

    auto removed_iterator = std::find(children_.begin(), children_.end(), child);
    ZAF_EXPECT(removed_iterator != children_.end());

    auto removed_index = std::distance(children_.begin(), removed_iterator);
    children_.erase(removed_iterator);

    if (set_parent_to_null) {
        child->SetParent(nullptr);
    }

    //The child's rect may be changed while calling NeedRelayout(), leading to a wrong repaint rect
    //while calling NeedRepaintRect(), so we preserve the original rect before calling 
    //NeedRelayout().
    auto child_rect = child->Rect();

    NeedRelayout();
    NeedRepaintRect(child_rect);

    AutoResizeToPreferredSize();

    auto inspector_port = GetInspectorPort();
    if (inspector_port) {
        inspector_port->ControlRemoveChild(shared_from_this(), removed_index);
    }
}


void Control::RemoveAllChildren() {

    std::shared_ptr<Control> focused_control;
    auto window = Window();
    if (window) {
        focused_control = window->FocusedControl();
    }

    for (const auto& each_child : children_) {

        //Remove focus before removing children.
        if (focused_control) {
            if (each_child->IsSameOrAncestorOf(*focused_control)) {
                focused_control->SetIsFocused(false);
            }
        }

        each_child->SetParent(nullptr);
    }

    children_.clear();
    NeedRepaint();
}


std::shared_ptr<Control> Control::FindChild(const std::wstring& name) const {

    for (const auto& each_child : children_) {
        if (each_child->Name() == name) {
            return each_child;
        }
    }

    for (const auto& each_child : children_) {
        auto found_child = each_child->FindChild(name);
        if (found_child) {
            return found_child;
        }
    }

    return {};
}


std::shared_ptr<Control> Control::InnerFindChildAtPosition(
    const Point& position, 
    bool recursively) const {

    auto content_rect = ContentRect();

    auto position_in_content = position;
    position_in_content.x -= content_rect.position.x;
    position_in_content.y -= content_rect.position.y;

    content_rect.position.x = 0;
    content_rect.position.y = 0;

    for (auto iterator = children_.rbegin(); iterator != children_.rend(); ++iterator) {

        const auto& child = *iterator;

        if (!child->IsVisible()) {
            continue;
        }

        zaf::Rect child_rect = child->Rect();
        child_rect.Intersect(content_rect);

        if (!child_rect.Contain(position_in_content)) {
            continue;
        }

        if (!recursively) {
            return child;
        }

        auto recursive_child = child->InnerFindChildAtPosition(
            TranslatePositionToChild(position, *child), 
            true);

        if (recursive_child) {
            return recursive_child;
        }

        return child;
    }

    return nullptr;
}


bool Control::IsParentOf(const Control& control) const {
    return control.Parent().get() == this;
}


bool Control::IsAncestorOf(const Control& control) const {

    auto ancestor = control.Parent();
    while (ancestor) {

        if (ancestor.get() == this) {
            return true;
        }

        ancestor = ancestor->Parent();
    }

    return false;
}


bool Control::IsSameOrAncestorOf(const Control& control) const {
    return (this == &control) || this->IsAncestorOf(control);
}


std::wstring Control::Name() const {
    return name_;
}


void Control::SetName(const std::wstring& name) {
    name_ = name;
}


std::shared_ptr<Window> Control::Window() const {

    auto parent = Parent();
    if (parent == nullptr) {
        return window_.lock();
    }

    return parent->Window();
}


bool Control::IsVisibleInContext() const {

    if (!IsVisible()) {
        return false;
    }

    //A control without parent is considered visible.
    auto parent = Parent();
    if (!parent) {
        return true;
    }

    return parent->IsVisibleInContext();
}


bool Control::IsVisible() const {
    return is_visible_;
}


void Control::SetIsVisible(bool is_visible) {

    if (IsVisible() == is_visible) {
        return;
    }

    SetInteractiveProperty(is_visible, is_visible_, &Control::OnIsVisibleChanged);

    //Notify parent to re-layout.
    auto parent = Parent();
    if (parent) {
        parent->NeedRelayout();
        parent->AutoResizeToPreferredSize();
    }
}


void Control::OnIsVisibleChanged() {
    is_visible_changed_event_.Raise(IsVisibleChangedInfo{ shared_from_this() });
}


Observable<IsVisibleChangedInfo> Control::IsVisibleChangedEvent() const {
    return is_visible_changed_event_.GetObservable();
}


bool Control::IsEnabledInContext() const {

    if (!IsEnabled()) {
        return false;
    }

    //A control without parent is considered enalbed.
    auto parent = Parent();
    if (!parent) {
        return true;
    }

    return parent->IsEnabledInContext();
}


bool Control::IsEnabled() const {
    return is_enabled_;
}


void Control::SetIsEnabled(bool is_enabled) {

    if (IsEnabled() == is_enabled) {
        return;
    }

    SetInteractiveProperty(is_enabled, is_enabled_, &Control::OnIsEnabledChanged);
}


void Control::OnIsEnabledChanged() {
    is_enabled_changed_event_.Raise(IsEnabledChangedInfo{ shared_from_this() });
}


Observable<IsEnabledChangedInfo> Control::IsEnabledChangedEvent() const {
    return is_enabled_changed_event_.GetObservable();
}


void Control::SetInteractiveProperty(
    bool new_value, 
    bool& property_value, 
    void(Control::*notification)()) {

    if (property_value == new_value) {
        return;
    }

    if (! new_value) {

        //Clear the mouse over control of window if needed.
        auto window = Window();
        if (window) {
            const auto& mouse_over_control = window->MouseOverControl();
            if (mouse_over_control) {
                if (this->IsSameOrAncestorOf(*mouse_over_control)) {
                    window->SetMouseOverControl(nullptr, MouseMessage{ Message{} });
                }
            }
        }

        SetIsFocused(false);
    }

    property_value = new_value;
    NeedRepaint();

    if (notification != nullptr) {
        (this->*notification)();
    }
}


bool Control::IsSelectedInContext() const {

    if (IsSelected()) {
        return true;
    }

    //Control is not selected if there is no parent.
    auto parent = Parent();
    if (!parent) {
        return false;
    }

    return parent->IsSelectedInContext();
}


bool Control::IsSelected() const {
    return is_selected_;
}


void Control::SetIsSelected(bool is_selected) {

    if (IsSelected() == is_selected) {
        return;
    }

    is_selected_ = is_selected;
    NeedRepaint();

    OnIsSelectedChanged();
}


void Control::OnIsSelectedChanged() {
    is_selected_changed_event_.Raise(IsSelectedChangedInfo{ shared_from_this() });
}


Observable<IsSelectedChangedInfo> Control::IsSelectedChangedEvent() const {
    return is_selected_changed_event_.GetObservable();
}


void Control::SetIsMouseOverByWindow(bool is_mouse_over) {
    is_mouse_over_ = is_mouse_over;
}


bool Control::ContainMouse() const {

    if (IsMouseOver()) {
        return true;
    }

    auto window = Window();
    if (!window) {
        return false;
    }

    const auto& mouse_over_control = window->MouseOverControl();
    if (!mouse_over_control) {
        return false;
    }

    return IsAncestorOf(*mouse_over_control);
}


bool Control::CanTabStop() const {
    return can_tab_stop_;
}


void Control::SetCanTabStop(bool can_tab_stop) {
    can_tab_stop_ = can_tab_stop;
}


std::optional<std::size_t> Control::TabIndex() const {
    return tab_index_;
}


void Control::SetTabIndex(std::size_t tab_index) {
    tab_index_ = tab_index;
}


void Control::SetIsFocused(bool is_focused) {

    if (!CanFocused()) {
        return;
    }

    if (IsFocused() == is_focused) {
        return;
    }

    auto window = Window();
    if (window != nullptr) {
        window->SetFocusedControl(is_focused ? shared_from_this() : nullptr);
    }
}


void Control::SetIsFocusedByWindow(bool is_focused) {
    is_focused_ = is_focused;
}


bool Control::ContainFocus() const {

    if (IsFocused()) {
        return true;
    }

    auto window = Window();
    if (!window) {
        return false;
    }

    const auto& focused_control = window->FocusedControl();
    if (!focused_control) {
        return false;
    }

    return IsAncestorOf(*focused_control);
}


void Control::SetIsCachedPaintingEnabled(bool value) {

    if (is_cached_painting_enabled_ == value) {
        return;
    }

    is_cached_painting_enabled_ = value;
    ReleaseCachedPaintingRenderer();
}


void Control::ReleaseCachedPaintingRenderer() {
    cached_renderer_ = {};
    valid_cached_renderer_rect_ = {};
}


bool Control::CanDoubleClick() const {
    return can_double_click_;
}

void Control::SetCanDoubleClick(bool can_double_click) {
    can_double_click_ = can_double_click;
}


std::wstring Control::Tooltip() const {
    return tooltip_;
}

void Control::SetTooltip(const std::wstring& tooltip) {
    tooltip_ = tooltip;
}


void Control::CaptureMouse() {

    auto window = Window();
    if (window != nullptr) {
        window->SetMouseCaptureControl(shared_from_this(), false);
    }
}


void Control::ReleaseMouse() {

    auto window = Window();
    if (window != nullptr) {
        window->SetMouseCaptureControl(shared_from_this(), true);
    }
}


void Control::IsCapturingMouseChanged(bool is_capturing_mouse) {

    is_capturing_mouse_ = is_capturing_mouse;

    internal::MouseCaptureEventInfo event_info{ shared_from_this() };
    if (is_capturing_mouse_) {
        OnMouseCaptured(event_info);
    }
    else {
        OnMouseReleased(event_info);
    }
}


const Point Control::GetMousePosition() const {

    auto window = Window();
    if (window == nullptr) {
        return Point();
    }
    
    Point mouse_position = window->GetMousePosition();
    Point absolute_position = *PositionInWindow();
    mouse_position.x -= absolute_position.x;
    mouse_position.y -= absolute_position.y;
    return mouse_position;
}


Point Control::TranslatePositionToParent(const Point& position) const {

    auto parent = Parent();
    ZAF_EXPECT(parent);

    const auto& this_position = Position();
    const auto& parent_border = parent->Border();
    const auto& parent_padding = parent->Padding();

    auto result = position;
    result.x += this_position.x + parent_border.left + parent_padding.left;
    result.y += this_position.y + parent_border.top + parent_padding.top;
    return result;
}


Point Control::TranslatePositionToChild(const Point& position, const Control& child) const {

    ZAF_EXPECT(child.Parent().get() == this);

    const auto& border = Border();
    const auto& padding = Padding();
    const auto& child_position = child.Position();

    Point result = position;
    result.x -= child_position.x + border.left + padding.left;
    result.y -= child_position.y + border.top + padding.top;
    return result;
}


std::optional<HitTestResult> Control::HitTest(const HitTestMessage& message) {

    auto parent = Parent();
    if (parent != nullptr) {
        return parent->HitTest(message);
    }
    else {
        return {};
    }
}


void Control::OnMouseCursorChanging(const MouseCursorChangingInfo& event_info) {
    mouse_cursor_changing_event_.Raise(event_info);
}


Observable<MouseCursorChangingInfo> Control::MouseCursorChangingEvent() const {
    return mouse_cursor_changing_event_.GetObservable();
}


void Control::OnPreMouseMove(const PreMouseMoveInfo& event_info) {
    pre_mouse_move_event_.Raise(event_info);
}


Observable<PreMouseMoveInfo> Control::PreMouseMoveEvent() const {
    return pre_mouse_move_event_.GetObservable();
}


void Control::OnMouseMove(const MouseMoveInfo& event_info) {
    mouse_move_event_.Raise(event_info);
}


Observable<MouseMoveInfo> Control::MouseMoveEvent() const {
    return mouse_move_event_.GetObservable();
}


void Control::OnMouseEnter(const MouseEnterInfo& event_info) {
    mouse_enter_event_.Raise(event_info);
}


Observable<MouseEnterInfo> Control::MouseEnterEvent() const {
    return mouse_enter_event_.GetObservable();
}


void Control::OnMouseLeave(const MouseLeaveInfo& event_info) {
    mouse_leave_event_.Raise(event_info);
}


Observable<MouseLeaveInfo> Control::MouseLeaveEvent() const {
    return mouse_leave_event_.GetObservable();
}


void Control::OnMouseHover(const MouseHoverInfo& event_info) {
    mouse_hover_event_.Raise(event_info);
}


Observable<MouseHoverInfo> Control::MouseHoverEvent() const {
    return mouse_hover_event_.GetObservable();
}


void Control::OnPreMouseDown(const PreMouseDownInfo& event_info) {
    pre_mouse_down_event_.Raise(event_info);
}


Observable<PreMouseDownInfo> Control::PreMouseDownEvent() const {
    return pre_mouse_down_event_.GetObservable();
}


void Control::OnMouseDown(const MouseDownInfo& event_info) {

    mouse_down_event_.Raise(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (event_info.Message().MouseButton() == MouseButton::Left) {
        if (HandleDoubleClickOnMouseDown(event_info.PositionAtSender())) {
            event_info.MarkAsHandled();
        }
    }
}


bool Control::HandleDoubleClickOnMouseDown(const Point& position) {

    if (!CanDoubleClick()) {
        return false;
    }

    //Check if need to raise double click.
    std::uint32_t current_time = GetTickCount();
    if (current_time - last_mouse_down_time_ <= GetDoubleClickTime()) {

        if (last_mouse_down_position_ == position) {
            last_mouse_down_time_ = 0;
            RaiseDoubleClickEvent(position);
            return true;
        }
    }

    last_mouse_down_time_ = current_time;
    last_mouse_down_position_ = position;
    return false;
}


Observable<MouseDownInfo> Control::MouseDownEvent() const {
    return mouse_down_event_.GetObservable();
}


void Control::OnPreMouseUp(const PreMouseUpInfo& event_info) {
    pre_mouse_up_event_.Raise(event_info);
}


Observable<PreMouseUpInfo> Control::PreMouseUpEvent() const {
    return pre_mouse_up_event_.GetObservable();
}


void Control::OnMouseUp(const MouseUpInfo& event_info) {
    mouse_up_event_.Raise(event_info);
}


Observable<MouseUpInfo> Control::MouseUpEvent() const {
    return mouse_up_event_.GetObservable();
}


void Control::RaiseDoubleClickEvent(const Point& position) {

    if (!CanDoubleClick()) {
        return;
    }

    DoubleClickInfo event_info{ shared_from_this(), position };
    OnDoubleClick(event_info);
}


void Control::OnDoubleClick(const DoubleClickInfo& event_info) {
    double_click_event_.Raise(event_info);
}


Observable<DoubleClickInfo> Control::DoubleClickEvent() const {
    return double_click_event_.GetObservable();
}


void Control::OnPreMouseWheel(const PreMouseWheelInfo& event_info) {
    pre_mouse_wheel_event_.Raise(event_info);
}


Observable<PreMouseWheelInfo> Control::PreMouseWheelEvent() const {
    return pre_mouse_wheel_event_.GetObservable();
}


void Control::OnMouseWheel(const MouseWheelInfo& event_info) {
    mouse_wheel_event_.Raise(event_info);
}


Observable<MouseWheelInfo> Control::MouseWheelEvent() const {
    return mouse_wheel_event_.GetObservable();
}


void Control::OnMouseCaptured(const MouseCapturedInfo& event_info) {
    mouse_captured_event_.Raise(event_info);
}


Observable<MouseCapturedInfo> Control::MouseCapturedEvent() const {
    return mouse_captured_event_.GetObservable();
}


void Control::OnMouseReleased(const MouseReleasedInfo& event_info) {
    mouse_released_event_.Raise(event_info);
}


Observable<MouseReleasedInfo> Control::MouseReleasedEvent() const {
    return mouse_released_event_.GetObservable();
}


void Control::OnPreKeyDown(const PreKeyDownInfo& event_info) {
    pre_key_down_event_.Raise(event_info);
}


Observable<PreKeyDownInfo> Control::PreKeyDownEvent() const {
    return pre_key_down_event_.GetObservable();
}


void Control::OnKeyDown(const KeyDownInfo& event_info) {
    key_down_event_.Raise(event_info);
}


Observable<KeyDownInfo> Control::KeyDownEvent() const {
    return key_down_event_.GetObservable();
}


void Control::OnPreKeyUp(const PreKeyUpInfo& event_info) {
    pre_key_up_event_.Raise(event_info);
}


Observable<PreKeyUpInfo> Control::PreKeyUpEvent() const {
    return pre_key_up_event_.GetObservable();
}


void Control::OnKeyUp(const KeyUpInfo& event_info) {
    key_up_event_.Raise(event_info);
}


Observable<KeyUpInfo> Control::KeyUpEvent() const {
    return key_up_event_.GetObservable();
}


void Control::OnPreCharInput(const PreCharInputInfo& event_info) {
    pre_char_input_event_.Raise(event_info);
}


Observable<PreCharInputInfo> Control::PreCharInputEvent() const {
    return pre_char_input_event_.GetObservable();
}


void Control::OnCharInput(const CharInputInfo& event_info) {
    char_input_event_.Raise(event_info);
}


Observable<CharInputInfo> Control::CharInputEvent() const {
    return char_input_event_.GetObservable();
}


void Control::OnPreSysKeyDown(const PreSysKeyDownInfo& event_info) {
    pre_sys_key_down_event_.Raise(event_info);
}


Observable<PreSysKeyDownInfo> Control::PreSysKeyDownEvent() const {
    return pre_sys_key_down_event_.GetObservable();
}


void Control::OnSysKeyDown(const SysKeyDownInfo& event_info) {
    sys_key_down_event_.Raise(event_info);
}


Observable<SysKeyDownInfo> Control::SysKeyDownEvent() const {
    return sys_key_down_event_.GetObservable();
}


void Control::OnPreSysKeyUp(const PreSysKeyUpInfo& event_info) {
    pre_sys_key_up_event_.Raise(event_info);
}


Observable<PreSysKeyUpInfo> Control::PreSysKeyUpEvent() const {
    return pre_sys_key_up_event_.GetObservable();
}


void Control::OnSysKeyUp(const SysKeyUpInfo& event_info) {
    sys_key_up_event_.Raise(event_info);
}


Observable<SysKeyUpInfo> Control::SysKeyUpEvent() const {
    return sys_key_up_event_.GetObservable();
}


void Control::OnPreSysCharInput(const PreSysCharInputInfo& event_info) {
    pre_sys_char_input_event_.Raise(event_info);
}


Observable<PreSysCharInputInfo> Control::PreSysCharInputEvent() const {
    return pre_sys_char_input_event_.GetObservable();
}


void Control::OnSysCharInput(const SysCharInputInfo& event_info) {
    sys_char_input_event_.Raise(event_info);
}


Observable<SysCharInputInfo> Control::SysCharInputEvent() const {
    return sys_char_input_event_.GetObservable();
}


void Control::OnFocusGained(const FocusGainedInfo& event_info) {
    focus_gained_event_.Raise(event_info);
}


Observable<FocusGainedInfo> Control::FocusGainedEvent() const {
    return focus_gained_event_.GetObservable();
}


void Control::OnFocusLost(const FocusLostInfo& event_info) {
    focus_lost_event_.Raise(event_info);
}


Observable<FocusLostInfo> Control::FocusLostEvent() const {
    return focus_lost_event_.GetObservable();
}


float Control::GetDPI() const {
    return GetDPIFromWindow(Window());
}


void Control::RaiseDPIChangedEvent() {

    OnDPIChanged();

    for (const auto& each_child : Children()) {
        each_child->RaiseDPIChangedEvent();
    }
}


void Control::OnDPIChanged() {

}


void Control::OnParentChanged(const ParentChangedInfo& event_info) {
    parent_changed_event_.Raise(event_info);
}


Observable<ParentChangedInfo> Control::ParentChangedEvent() const {
    return parent_changed_event_.GetObservable();
}


void Control::RaiseWindowChangedEvent(const std::shared_ptr<zaf::Window>& previous_window) {

    WindowChangedInfo window_changed_info{ shared_from_this(), previous_window };
    OnWindowChanged(window_changed_info);

    for (const auto& each_child : Children()) {
        each_child->RaiseWindowChangedEvent(previous_window);
    }
}


void Control::OnWindowChanged(const WindowChangedInfo& event_info) {
    window_changed_event_.Raise(event_info);
}


Observable<WindowChangedInfo> Control::WindowChangedEvent() const {
    return window_changed_event_.GetObservable();
}


std::shared_ptr<internal::InspectorPort> Control::GetInspectorPort() const {

    auto window = Window();
    if (!window) {
        return nullptr;
    }

    return window->GetInspectorPort();
}

}