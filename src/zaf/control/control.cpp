#include <zaf/control/control.h>
#include <algorithm>
#include <zaf/application.h>
#include <zaf/base/as.h>
#include <zaf/base/auto_reset.h>
#include <zaf/base/define.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/control/internal/cached_painting.h>
#include <zaf/control/internal/control_facets/control_geometry_facet.h>
#include <zaf/control/internal/control_facets/control_hierarchy_facet.h>
#include <zaf/control/internal/control_facets/control_mouse_facet.h>
#include <zaf/control/internal/control_updating.h>
#include <zaf/control/internal/image_box/image_drawing.h>
#include <zaf/control/layout/anchor_layouter.h>
#include <zaf/graphic/pixel_snapping.h>
#include <zaf/graphic/canvas/canvas.h>
#include <zaf/graphic/font.h>
#include <zaf/graphic/d2d/path_geometry.h>
#include <zaf/graphic/d2d/rectangle_geometry.h>
#include <zaf/graphic/image.h>
#include <zaf/internal/theme.h>
#include <zaf/rx/subject/subject.h>
#include <zaf/window/inspector/internal/inspector_port.h>
#include <zaf/window/internal/window_facets/window_focus_facet.h>
#include <zaf/window/internal/window_facets/window_inspect_facet.h>
#include <zaf/window/internal/window_facets/window_mouse_facet.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>
#include <zaf/window/window.h>

#undef max
#undef min

namespace zaf {
namespace {

float GetDPIFromWindow(const std::shared_ptr<Window>& window) {
    if (window) {
        return window->DPI();
    }
    return Application::Instance().GetSystemDPI();
}

constexpr bool DefaultIsEnabled = true;
constexpr bool DefaultIsVisible = true;

}


Control::Control() : 
    geometry_facet_(std::make_unique<internal::ControlGeometryFacet>(*this)),
    hierarchy_facet_(std::make_unique<internal::ControlHierarchyFacet>(*this)),
    mouse_facet_(std::make_unique<internal::ControlMouseFacet>(*this)),
    is_focused_(false),
    is_enabled_(DefaultIsEnabled),
    is_visible_(DefaultIsVisible) {

}


Control::~Control() {

}


#pragma region Geometry Management

const zaf::Rect& Control::Rect() const noexcept {
    return geometry_facet_->Rect();
}


void Control::SetRect(const zaf::Rect& rect) {
    geometry_facet_->SetRect(rect);
}


zaf::Rect Control::RectInSelf() const noexcept {
    return geometry_facet_->RectInSelf();
}


std::optional<zaf::Rect> Control::RectInWindow() const noexcept {
    return geometry_facet_->RectInWindow();
}


const Point& Control::Position() const noexcept {
    return geometry_facet_->Position();
}


void Control::SetPosition(const Point& position) {
    geometry_facet_->SetPosition(position);
}


std::optional<zaf::Point> Control::PositionInWindow() const noexcept {
    return geometry_facet_->PositionInWindow();
}


float Control::X() const noexcept {
    return geometry_facet_->X();
}


void Control::SetX(float x) {
    geometry_facet_->SetX(x);
}


float Control::Y() const noexcept {
    return geometry_facet_->Y();
}


void Control::SetY(float y) {
    geometry_facet_->SetY(y);
}


const zaf::Size& Control::Size() const noexcept {
    return geometry_facet_->Size();
}


void Control::SetSize(const zaf::Size& size) {
    geometry_facet_->SetSize(size);
}


float Control::Width() const noexcept {
    return geometry_facet_->Width();
}


void Control::SetWidth(float width) {
    geometry_facet_->SetWidth(width);
}


float Control::Height() const noexcept {
    return geometry_facet_->Height();
}


void Control::SetHeight(float height) {
    geometry_facet_->SetHeight(height);
}


float Control::MinWidth() const noexcept {
    return geometry_facet_->MinWidth();
}


void Control::SetMinWidth(float min_width) {
    geometry_facet_->SetMinWidth(min_width);
}


float Control::MaxWidth() const noexcept {
    return geometry_facet_->MaxWidth();
}


void Control::SetMaxWidth(float max_width) {
    geometry_facet_->SetMaxWidth(max_width);
}


void Control::SetFixedWidth(float width) {
    geometry_facet_->SetFixedWidth(width);
}


float Control::MinHeight() const noexcept {
    return geometry_facet_->MinHeight();
}


void Control::SetMinHeight(float min_height) {
    geometry_facet_->SetMinHeight(min_height);
}


float Control::MaxHeight() const noexcept {
    return geometry_facet_->MaxHeight();
}


void Control::SetMaxHeight(float max_height) {
    geometry_facet_->SetMaxHeight(max_height);
}


void Control::SetFixedHeight(float height) {
    geometry_facet_->SetFixedHeight(height);
}


void Control::SetFixedSize(const zaf::Size& size) {
    geometry_facet_->SetFixedSize(size);
}


zaf::Size Control::CalculatePreferredSize() const {
    return geometry_facet_->CalculatePreferredSize();
}


zaf::Size Control::CalculatePreferredSize(const zaf::Size& bound_size) const {
    return geometry_facet_->CalculatePreferredSize(bound_size);
}


bool Control::AutoWidth() const {
    return geometry_facet_->AutoWidth();
}


void Control::SetAutoWidth(bool value) {
    geometry_facet_->SetAutoWidth(value);
}


bool Control::AutoHeight() const {
    return geometry_facet_->AutoHeight();
}


void Control::SetAutoHeight(bool value) {
    geometry_facet_->SetAutoHeight(value);
}


void Control::SetAutoSize(bool value) {
    geometry_facet_->SetAutoSize(value);
}


float Control::ClampWidth(float width) const noexcept {
    return geometry_facet_->ClampWidth(width);
}


float Control::ClampHeight(float height) const noexcept {
    return geometry_facet_->ClampHeight(height);
}


zaf::Size Control::ClampSize(const zaf::Size& size) const noexcept {
    return geometry_facet_->ClampSize(size);
}


Anchor Control::Anchor() const {
    return geometry_facet_->Anchor();
}


void Control::SetAnchor(zaf::Anchor anchor) {
    geometry_facet_->SetAnchor(anchor);
}


const Frame& Control::Margin() const {
    return geometry_facet_->Margin();
}


void Control::SetMargin(const Frame& margin) {
    geometry_facet_->SetMargin(margin);
}


const Frame& Control::Border() const {
    return geometry_facet_->Border();
}


void Control::SetBorder(const Frame& border) {
    geometry_facet_->SetBorder(border);
}


const Frame& Control::Padding() const {
    return geometry_facet_->Padding();
}


void Control::SetPadding(const Frame& padding) {
    geometry_facet_->SetPadding(padding);
}


std::optional<zaf::Rect> Control::ContentRectInWindow() const noexcept {
    return geometry_facet_->ContentRectInWindow();
}


zaf::Rect Control::ContentRectInSelf() const noexcept {
    return geometry_facet_->ContentRectInSelf();
}


zaf::Rect Control::ContentRect() const noexcept {
    return geometry_facet_->ContentRect();
}


zaf::Size Control::ContentSize() const {
    return geometry_facet_->ContentSize();
}


void Control::OnRectChanged(const RectChangedInfo& event_info) {
    rect_changed_event_.Raise(event_info);
}


rx::Observable<RectChangedInfo> Control::RectChangedEvent() const {
    return rect_changed_event_.GetObservable();
}


void Control::OnPositionChanged(const PositionChangedInfo& event_info) {
    position_changed_event_.Raise(event_info);
}


rx::Observable<PositionChangedInfo> Control::PositionChangedEvent() const {
    return position_changed_event_.GetObservable();
}


void Control::OnSizeChanged(const SizeChangedInfo& event_info) {
    size_changed_event_.Raise(event_info);
}


rx::Observable<SizeChangedInfo> Control::SizeChangedEvent() const {
    return size_changed_event_.GetObservable();
}

#pragma endregion


#pragma region Hierarchy Management

std::shared_ptr<Window> Control::Window() const noexcept {
    return hierarchy_facet_->Window();
}


void Control::SetWindow(const std::shared_ptr<zaf::Window>& window) noexcept {
    hierarchy_facet_->SetDirectWindow(window);
}


rx::Observable<WindowChangedInfo> Control::WindowChangedEvent() const {
    return window_changed_event_.GetObservable();
}


std::shared_ptr<Control> Control::Parent() const noexcept {
    return hierarchy_facet_->Parent();
}


rx::Observable<ParentChangedInfo> Control::ParentChangedEvent() const {
    return parent_changed_event_.GetObservable();
}


bool Control::IsParentOf(const Control& control) const noexcept {
    return hierarchy_facet_->IsParentOf(control);
}


bool Control::IsAncestorOf(const Control& control) const noexcept {
    return hierarchy_facet_->IsAncestorOf(control);
}


bool Control::IsSameOrAncestorOf(const Control& control) const noexcept {
    return hierarchy_facet_->IsSameOrAncestorOf(control);
}


bool Control::HasChildren() const noexcept {
    return !hierarchy_facet_->Children().empty();
}


std::size_t Control::ChildCount() const noexcept {
    return hierarchy_facet_->Children().size();
}


std::shared_ptr<Control> Control::GetChildAtIndex(std::size_t index) const noexcept {
    return hierarchy_facet_->GetChildAtIndex(index);
}


const std::vector<std::shared_ptr<Control>>& Control::Children() const noexcept {
    return hierarchy_facet_->Children();
}


void Control::SetChildren(const std::vector<std::shared_ptr<Control>>& children) {
    hierarchy_facet_->SetChildren(children);
}


void Control::AddChild(const std::shared_ptr<Control>& child) {
    hierarchy_facet_->AddChild(child);
}


void Control::AddChildAtIndex(const std::shared_ptr<Control>& child, std::size_t index) {
    hierarchy_facet_->AddChildAtIndex(child, index);
}


void Control::AddChildren(const std::vector<std::shared_ptr<Control>>& children) {
    hierarchy_facet_->AddChildren(children);
}


void Control::AddChildrenAtIndex(
    const std::vector<std::shared_ptr<Control>>& children,
    std::size_t index) {

    hierarchy_facet_->AddChildrenAtIndex(children, index);
}


void Control::RemoveChild(const std::shared_ptr<Control>& child) {
    hierarchy_facet_->RemoveChild(child);
}


void Control::RemoveChildAtIndex(std::size_t index) {
    hierarchy_facet_->RemoveChildAtIndex(index);
}


void Control::RemoveAllChildren() {
    hierarchy_facet_->RemoveAllChildren();
}


std::shared_ptr<Control> Control::FindChild(std::wstring_view name) const noexcept {
    return hierarchy_facet_->FindChild(name);
}


std::shared_ptr<Control> Control::FindDescendant(std::wstring_view name) const noexcept {
    return hierarchy_facet_->FindDescendant(name);
}


std::shared_ptr<Control> Control::FindChildAtPosition(const Point& position) const noexcept {
    return hierarchy_facet_->FindChildAtPosition(position);
}


std::shared_ptr<Control> Control::FindDescendantAtPosition(
    const Point& position) const noexcept {

    return hierarchy_facet_->FindDescendantAtPosition(position);
}

#pragma endregion


#pragma region Mouse Input Handling

bool Control::IsMouseOver() const noexcept {
    return mouse_facet_->IsMouseOver();
}


bool Control::ContainsMouse() const noexcept {
    return mouse_facet_->ContainsMouse();
}


rx::Observable<MouseEnterInfo> Control::MouseEnterEvent() const {
    return mouse_enter_event_.GetObservable();
}


rx::Observable<MouseLeaveInfo> Control::MouseLeaveEvent() const {
    return mouse_leave_event_.GetObservable();
}


rx::Observable<MouseHoverInfo> Control::MouseHoverEvent() const {
    return mouse_hover_event_.GetObservable();
}


rx::Observable<MouseCursorChangingInfo> Control::MouseCursorChangingEvent() const {
    return mouse_cursor_changing_event_.GetObservable();
}


rx::Observable<PreMouseMoveInfo> Control::PreMouseMoveEvent() const {
    return pre_mouse_move_event_.GetObservable();
}


rx::Observable<MouseMoveInfo> Control::MouseMoveEvent() const {
    return mouse_move_event_.GetObservable();
}


rx::Observable<PreMouseDownInfo> Control::PreMouseDownEvent() const {
    return pre_mouse_down_event_.GetObservable();
}


rx::Observable<MouseDownInfo> Control::MouseDownEvent() const {
    return mouse_down_event_.GetObservable();
}


rx::Observable<PreMouseUpInfo> Control::PreMouseUpEvent() const {
    return pre_mouse_up_event_.GetObservable();
}


rx::Observable<MouseUpInfo> Control::MouseUpEvent() const {
    return mouse_up_event_.GetObservable();
}


rx::Observable<PreMouseWheelInfo> Control::PreMouseWheelEvent() const {
    return pre_mouse_wheel_event_.GetObservable();
}


rx::Observable<MouseWheelInfo> Control::MouseWheelEvent() const {
    return mouse_wheel_event_.GetObservable();
}


bool Control::CanDoubleClick() const noexcept {
    return mouse_facet_->CanDoubleClick();
}


void Control::SetCanDoubleClick(bool can_double_click) noexcept {
    mouse_facet_->SetCanDoubleClick(can_double_click);
}


rx::Observable<DoubleClickInfo> Control::DoubleClickEvent() const {
    return double_click_event_.GetObservable();
}


bool Control::CaptureMouse() {
    return mouse_facet_->CaptureMouse();
}


rx::Observable<MouseCapturedInfo> Control::MouseCapturedEvent() const {
    return mouse_captured_event_.GetObservable();
}


void Control::ReleaseMouse() {
    mouse_facet_->ReleaseMouse();
}


rx::Observable<MouseReleasedInfo> Control::MouseReleasedEvent() const {
    return mouse_released_event_.GetObservable();
}


bool Control::HasMouseCapture() const noexcept {
    return mouse_facet_->HasMouseCapture();
}


const Point Control::GetMousePosition() const {
    return mouse_facet_->GetMousePosition();
}


std::optional<HitTestResult> Control::HitTest(const HitTestMessage& message) {
    return mouse_facet_->HitTest(message);
}


void Control::OnMouseCursorChanging(const MouseCursorChangingInfo& event_info) {
    mouse_cursor_changing_event_.Raise(event_info);
}


void Control::OnPreMouseMove(const PreMouseMoveInfo& event_info) {
    pre_mouse_move_event_.Raise(event_info);
}


void Control::OnMouseMove(const MouseMoveInfo& event_info) {
    mouse_move_event_.Raise(event_info);
}


void Control::OnMouseEnter(const MouseEnterInfo& event_info) {
    mouse_enter_event_.Raise(event_info);
}


void Control::OnMouseLeave(const MouseLeaveInfo& event_info) {
    mouse_leave_event_.Raise(event_info);
}


void Control::OnMouseHover(const MouseHoverInfo& event_info) {
    mouse_hover_event_.Raise(event_info);
}


void Control::OnPreMouseDown(const PreMouseDownInfo& event_info) {
    pre_mouse_down_event_.Raise(event_info);
}


void Control::OnMouseDown(const MouseDownInfo& event_info) {
    mouse_facet_->OnMouseDown(event_info);
}


void Control::OnPreMouseUp(const PreMouseUpInfo& event_info) {
    pre_mouse_up_event_.Raise(event_info);
}


void Control::OnMouseUp(const MouseUpInfo& event_info) {
    mouse_up_event_.Raise(event_info);
}


void Control::OnDoubleClick(const DoubleClickInfo& event_info) {
    double_click_event_.Raise(event_info);
}


void Control::OnPreMouseWheel(const PreMouseWheelInfo& event_info) {
    pre_mouse_wheel_event_.Raise(event_info);
}


void Control::OnMouseWheel(const MouseWheelInfo& event_info) {
    mouse_wheel_event_.Raise(event_info);
}


void Control::OnMouseCaptured(const MouseCapturedInfo& event_info) {
    mouse_captured_event_.Raise(event_info);
}


void Control::OnMouseReleased(const MouseReleasedInfo& event_info) {
    mouse_released_event_.Raise(event_info);
}

#pragma endregion


std::shared_ptr<Image> Control::BackgroundImage() const {
    return BackgroundImagePicker()(*this);
}


void Control::SetBackgroundImage(const std::shared_ptr<Image>& image) {
    SetBackgroundImagePicker(CreateImagePicker(image));
}


bool Control::CanFocus() const {
    return can_focus_;
}


void Control::SetCanFocus(bool can_focused) {
    can_focus_ = can_focused;
}


bool Control::IsFocused() const {
    return is_focused_;
}


bool Control::IsCachedPaintingEnabled() const {
    return is_cached_painting_enabled_;
}


bool Control::AcceptKeyMessage(const KeyMessage& message) {
    return false;
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


void Control::Initialize() {

    __super::Initialize();

    SetBackgroundColor(Color::Transparent());
    SetBorderColor(Color::Black());
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
        RequestLayout();
    }

    if (!update_state->need_repainted_rect.IsEmpty()) {
        NeedRepaintRect(update_state->need_repainted_rect);
    }

    if (update_state->need_resize) {
        AutoResizeToPreferredSize();
    }
}


void Control::NeedUpdateStyle() {
    need_update_style_ = true;
}


void Control::UpdateStyle() {
    
    background_color_field_.UpdateColor(*this);
    border_color_field_.UpdateColor(*this);
}


rx::Observable<StyleUpdateInfo> Control::StyleUpdateEvent() const {
    return style_update_event_.GetObservable();
}


void Control::OnStyleUpdate(const StyleUpdateInfo& event_info) {
    style_update_event_.Raise(event_info);
}


void Control::Repaint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    //Make sure the control repaints only if it is visible.
    ZAF_EXPECT(IsVisible());

    bool need_update_style = HandleUpdateStyle();

    if (IsCachedPaintingEnabled()) {
        RepaintUsingCachedPainting(canvas, dirty_rect);
    }
    else {
        RepaintControl(canvas, dirty_rect, false, need_update_style);
    }
}


bool Control::HandleUpdateStyle() {

    if (!need_update_style_) {
        return false;
    }

    need_update_style_ = false;

    auto auto_reset = MakeAutoReset(is_updating_style_, true);

    UpdateStyle();
    OnStyleUpdate(StyleUpdateInfo{ shared_from_this() });
    return true;
}


void Control::RepaintUsingCachedPainting(Canvas& canvas, const zaf::Rect& dirty_rect) {

    auto control_size = Size();
    control_size.width = std::ceil(control_size.width);
    control_size.height = std::ceil(control_size.height);

    //Create the cached renderer if it is not created.
    if (!cached_renderer_) {
        d2d::CompatibleRendererProperties properties;
        properties.DesiredSize(control_size);
        cached_renderer_ = canvas.Renderer().CreateCompatibleRenderer(properties);
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

            RepaintControl(
                cached_painting_canvas,
                calculate_result.actual_dirty_rect, 
                true, 
                false);
        }
        cached_renderer_.EndDraw();

        //Update valid cached rect after repainting.
        valid_cached_renderer_rect_ = calculate_result.new_valid_cached_rect;
    }

    //Paint into canvas from cached renderer.
    DrawImageOptions options;
    options.source_rect = dirty_rect;
    canvas.DrawBitmap(cached_renderer_.GetBitmap(), dirty_rect, options);
}


void Control::RepaintControl(
    Canvas& canvas, 
    const zaf::Rect& dirty_rect,
    bool need_clear,
    bool update_style) {

    if (need_clear) {
        canvas.Clear();
    }
    Paint(canvas, dirty_rect);

    RepaintChildren(canvas, dirty_rect, update_style);
}


void Control::RepaintChildren(Canvas& canvas, const zaf::Rect& dirty_rect, bool update_style) {

    //No need to repaint if there is no child.
    const auto& children = hierarchy_facet_->Children();
    if (children.empty()) {
        return;
    }

    //No need to repaint if the content rect is not dirty.
    zaf::Rect content_rect = ContentRectInSelf();
    if (!content_rect.HasIntersection(dirty_rect)) {
        return;
    }

    for (const auto& child : children) {

        //Don't paint the child if it is not visible.
        if (!child->IsVisible()) {
            continue;
        }

        //Children should update their styles if parent needs update as well.
        if (update_style) {
            child->NeedUpdateStyle();
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


void Control::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const {

    auto state_guard = canvas.PushState();

    zaf::Rect control_rect{ Point{}, Size() };

    //Draw background color.
    state_guard.SetBrush(BackgroundColor());
    canvas.FillRectangle(control_rect);

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

    auto control_geometry = canvas.CreateSnappedRectangleGeometry(control_rect);
    auto inner_geometry = canvas.CreateSnappedRectangleGeometry(inner_rect);
    
    auto border_geometry = canvas.CreateSnappedPathGeometry();
    auto sink = border_geometry.Open();
    d2d::Geometry::Combine(
        control_geometry, 
        inner_geometry, 
        d2d::Geometry::CombineMode::Exclude, 
        sink);
    sink.Close();

    state_guard.SetBrush(BorderColor());
    canvas.FillGeometry(border_geometry);
}


void Control::DrawBackgroundImage(Canvas& canvas, const zaf::Rect& background_rect) const {

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
        d2d::InterpolationMode::Linear);
}


void Control::NeedRepaint() {
    NeedRepaintRect(zaf::Rect(Point(), Size()));
}


void Control::NeedRepaintRect(const zaf::Rect& rect) {

    //Ignore repaint request when the control is updating style, as the style is updated only 
    //before painting.
    if (is_updating_style_) {
        return;
    }

    if ((rect.size.width == 0) || (rect.size.height == 0)) {
        return;
    }

    if (update_state_) {
        update_state_->need_repainted_rect.Union(rect);
        return;
    }

    zaf::Rect bound_rect(Point(), Size());
    zaf::Rect repaint_rect = zaf::Rect::Intersect(bound_rect, rect);
    if (repaint_rect.IsEmpty()) {
        return;
    }

    RecalculateCachedPaintingRect(repaint_rect);

    auto window = hierarchy_facet_->DirectWindow();
    if (window != nullptr) {
        window->RequestRepaint(repaint_rect);
        return;
    }

    auto parent = Parent();
    if (parent == nullptr) {
        return;
    }

    Point position_in_parent = TranslateToParent(repaint_rect.position);

    zaf::Rect repaint_rect_in_parent(position_in_parent, repaint_rect.size);
    repaint_rect_in_parent.Intersect(parent->ContentRectInSelf());
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

    for (const auto& each_child : hierarchy_facet_->Children()) {
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

    if (!is_layouting_ || (AutoWidth() || AutoHeight())) {
        RequestLayout();
    }
    AutoResizeToPreferredSize();
}


void Control::Layout(const zaf::Size& previous_size) {

    auto layouter = Layouter();
    if (layouter) {
        layouter->Layout(*this, previous_size, Children());
    }
}


void Control::RequestLayout() {
    RequestLayout(Size());
}


void Control::RequestLayout(const zaf::Size& previous_size) {

    ZAF_EXPECT(!is_updating_style_);

    if (update_state_) {
        update_state_->need_relayout = true;
    }
    else {

        //Avoid auto resize when layouting children.
        auto update_guard = BeginUpdate();

        auto auto_reset = MakeAutoReset(is_layouting_, true);
        Layout(previous_size);
    }
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


void Control::AutoResizeToPreferredSize() {
    geometry_facet_->AutoResizeToPreferredSize();
}


void Control::RaiseContentChangedEvent() {
    AutoResizeToPreferredSize();
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


Color Control::BackgroundColor() const {
    return background_color_field_.Color();
}


void Control::SetBackgroundColor(const Color& color) {
    background_color_field_.SetColor(color, *this);
}


const ColorPicker& Control::BackgroundColorPicker() const {
    return background_color_field_.ColorPicker();
}

void Control::SetBackgroundColorPicker(ColorPicker picker) {
    background_color_field_.SetColorPicker(std::move(picker), *this);
}


Color Control::BorderColor() const {
    return border_color_field_.Color();
}


void Control::SetBorderColor(const Color& color) {
    border_color_field_.SetColor(color, *this);
}


const ColorPicker& Control::BorderColorPicker() const {
    return border_color_field_.ColorPicker();
}

void Control::SetBorderColorPicker(ColorPicker picker) {
    border_color_field_.SetColorPicker(std::move(picker), *this);
}


std::shared_ptr<Layouter> Control::Layouter() const {

    if (layouter_) {
        return layouter_;
    }
    return GetAnchorLayouter();
}

void Control::SetLayouter(const std::shared_ptr<zaf::Layouter>& layouter) {
    layouter_ = layouter;
    RequestLayout();
}


std::wstring Control::Name() const {
    return name_;
}


void Control::SetName(const std::wstring& name) {
    name_ = name;
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
        parent->RequestLayout();
        parent->AutoResizeToPreferredSize();
    }
}


void Control::OnIsVisibleChanged() {
    is_visible_changed_event_.Raise(IsVisibleChangedInfo{ shared_from_this() });
}


rx::Observable<IsVisibleChangedInfo> Control::IsVisibleChangedEvent() const {
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


rx::Observable<IsEnabledChangedInfo> Control::IsEnabledChangedEvent() const {
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
                    window->mouse_facet_->SetMouseOverControl(nullptr, MouseMessage{ Message{} });
                }
            }
        }

        SetIsFocused(false);
    }

    property_value = new_value;
    NeedUpdateStyle();
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
    NeedUpdateStyle();
    NeedRepaint();

    OnIsSelectedChanged();
}


void Control::OnIsSelectedChanged() {
    is_selected_changed_event_.Raise(IsSelectedChangedInfo{ shared_from_this() });
}


rx::Observable<IsSelectedChangedInfo> Control::IsSelectedChangedEvent() const {
    return is_selected_changed_event_.GetObservable();
}


void Control::SetIsMouseOverByWindow(bool is_mouse_over) {
    mouse_facet_->SetIsMouseOver(is_mouse_over);
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

    if (!CanFocus()) {
        return;
    }

    auto old_is_focused = IsFocused();
    if (old_is_focused == is_focused) {
        return;
    }

    auto window = Window();
    if (!window) {
        return;
    }

    window->focus_facet_->ChangeFocusedControl(
        is_focused ? shared_from_this() : nullptr);
}


void Control::SetIsFocusedByWindow(bool is_focused) {

    is_focused_ = is_focused;

    NeedUpdateStyle();
    NeedRepaint();
}


bool Control::IsInFocusedContext() const {
    
    if (IsFocused()) {
        return true;
    }

    auto parent = Parent();
    if (!parent) {
        return false;
    }

    return parent->IsInFocusedContext();
}


bool Control::ContainsFocus() const {

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


std::wstring Control::Tooltip() const {
    return tooltip_;
}

void Control::SetTooltip(const std::wstring& tooltip) {
    tooltip_ = tooltip;
}


void Control::IsCapturingMouseChanged(bool is_capturing_mouse) {
    mouse_facet_->IsCapturingMouseChanged(is_capturing_mouse);
}


Point Control::TranslateToParent(const Point& position) const {

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


Point Control::TranslateFromParent(const Point& position) const {

    auto parent = Parent();
    ZAF_EXPECT(parent);

    const auto& this_position = Position();
    const auto& parent_border = parent->Border();
    const auto& parent_padding = parent->Padding();

    Point result = position;
    result.x -= this_position.x + parent_border.left + parent_padding.left;
    result.y -= this_position.y + parent_border.top + parent_padding.top;
    return result;
}


void Control::OnPreKeyDown(const PreKeyDownInfo& event_info) {
    pre_key_down_event_.Raise(event_info);
}


rx::Observable<PreKeyDownInfo> Control::PreKeyDownEvent() const {
    return pre_key_down_event_.GetObservable();
}


void Control::OnKeyDown(const KeyDownInfo& event_info) {
    key_down_event_.Raise(event_info);
}


rx::Observable<KeyDownInfo> Control::KeyDownEvent() const {
    return key_down_event_.GetObservable();
}


void Control::OnPreKeyUp(const PreKeyUpInfo& event_info) {
    pre_key_up_event_.Raise(event_info);
}


rx::Observable<PreKeyUpInfo> Control::PreKeyUpEvent() const {
    return pre_key_up_event_.GetObservable();
}


void Control::OnKeyUp(const KeyUpInfo& event_info) {
    key_up_event_.Raise(event_info);
}


rx::Observable<KeyUpInfo> Control::KeyUpEvent() const {
    return key_up_event_.GetObservable();
}


void Control::OnPreCharInput(const PreCharInputInfo& event_info) {
    pre_char_input_event_.Raise(event_info);
}


rx::Observable<PreCharInputInfo> Control::PreCharInputEvent() const {
    return pre_char_input_event_.GetObservable();
}


void Control::OnCharInput(const CharInputInfo& event_info) {
    char_input_event_.Raise(event_info);
}


rx::Observable<CharInputInfo> Control::CharInputEvent() const {
    return char_input_event_.GetObservable();
}


void Control::OnPreSysKeyDown(const PreSysKeyDownInfo& event_info) {
    pre_sys_key_down_event_.Raise(event_info);
}


rx::Observable<PreSysKeyDownInfo> Control::PreSysKeyDownEvent() const {
    return pre_sys_key_down_event_.GetObservable();
}


void Control::OnSysKeyDown(const SysKeyDownInfo& event_info) {
    sys_key_down_event_.Raise(event_info);
}


rx::Observable<SysKeyDownInfo> Control::SysKeyDownEvent() const {
    return sys_key_down_event_.GetObservable();
}


void Control::OnPreSysKeyUp(const PreSysKeyUpInfo& event_info) {
    pre_sys_key_up_event_.Raise(event_info);
}


rx::Observable<PreSysKeyUpInfo> Control::PreSysKeyUpEvent() const {
    return pre_sys_key_up_event_.GetObservable();
}


void Control::OnSysKeyUp(const SysKeyUpInfo& event_info) {
    sys_key_up_event_.Raise(event_info);
}


rx::Observable<SysKeyUpInfo> Control::SysKeyUpEvent() const {
    return sys_key_up_event_.GetObservable();
}


void Control::OnPreSysCharInput(const PreSysCharInputInfo& event_info) {
    pre_sys_char_input_event_.Raise(event_info);
}


rx::Observable<PreSysCharInputInfo> Control::PreSysCharInputEvent() const {
    return pre_sys_char_input_event_.GetObservable();
}


void Control::OnSysCharInput(const SysCharInputInfo& event_info) {
    sys_char_input_event_.Raise(event_info);
}


rx::Observable<SysCharInputInfo> Control::SysCharInputEvent() const {
    return sys_char_input_event_.GetObservable();
}


void Control::OnPreFocusGained(const PreFocusGainedInfo& event_info){
    pre_focus_gained_event_.Raise(event_info);
}


rx::Observable<PreFocusGainedInfo> Control::PreFocusGainedEvent() const {
    return pre_focus_gained_event_.GetObservable();
}


void Control::OnFocusGained(const FocusGainedInfo& event_info) {
    focus_gained_event_.Raise(event_info);
}


rx::Observable<FocusGainedInfo> Control::FocusGainedEvent() const {
    return focus_gained_event_.GetObservable();
}


void Control::OnPreFocusLost(const PreFocusLostInfo& event_info) {
    pre_focus_lost_event_.Raise(event_info);
}


rx::Observable<PreFocusLostInfo> Control::PreFocusLostEvent() const {
    return pre_focus_lost_event_.GetObservable();
}


void Control::OnFocusLost(const FocusLostInfo& event_info) {
    focus_lost_event_.Raise(event_info);
}


rx::Observable<FocusLostInfo> Control::FocusLostEvent() const {
    return focus_lost_event_.GetObservable();
}


void Control::OnIMEStartComposition(const IMEStartCompositionInfo& event_info) {

}


void Control::OnIMEComposition(const IMECompositionInfo& event_info) {

}


void Control::OnIMEEndComposition(const IMEEndCompositionInfo& event_info) {


}


void Control::OnIMERequest(const IMERequestInfo& event_info) {

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


std::shared_ptr<internal::InspectorPort> Control::GetInspectorPort() const {

    auto window = Window();
    if (!window) {
        return nullptr;
    }

    return window->inspect_facet_->GetInspectorPort();
}

}