#include <zaf/control/control.h>
#include <algorithm>
#include <zaf/application.h>
#include <zaf/base/as.h>
#include <zaf/base/define.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/base/error/check.h>
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
#include <zaf/object/parsing/xaml_node_parse_helper.h>
#include <zaf/object/type_definition.h>
#include <zaf/rx/subject.h>
#include <zaf/serialization/properties.h>
#include <zaf/window/inspector/internal/inspector_port.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>
#include <zaf/window/window.h>

#undef max
#undef min

namespace zaf {
namespace {

class ControlParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) override {

    }

    void ParseFromNode(const XamlNode& node, Object& object) override {

        auto& control = As<Control>(object);
        auto update_guard = control.BeginUpdate();

        XamlNodeParseHelper helper(node, control.GetType());
        auto tab_index = helper.GetFloatProperty(L"TabIndex");
        if (tab_index) {
            control.SetTabIndex(static_cast<std::size_t>(*tab_index));
        }

        for (const auto& each_node : node.GetContentNodes()) {

            auto child_control = internal::CreateObjectFromNode<Control>(each_node);
            if (!child_control) {
                ZAF_THROW_ERRC(BasicErrc::NameNotFound);
            }

            control.AddChild(child_control);
        }
    }
};

constexpr bool DefaultCanFocused = false;
constexpr bool DefaultIsEnabled = true;
constexpr bool DefaultIsVisible = true;

}


ZAF_DEFINE_TYPE(Control)
ZAF_DEFINE_TYPE_PARSER(ControlParser)
ZAF_DEFINE_TYPE_PROPERTY(Rect)
ZAF_DEFINE_TYPE_PROPERTY(AbsoluteContentRect)
ZAF_DEFINE_TYPE_PROPERTY(AbsoluteRect)
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
ZAF_DEFINE_TYPE_PROPERTY(IsSelfVisible)
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
    ZAF_EXPECT(IsSelfVisible());

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
    if (cached_renderer_ == nullptr) {
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
        if (!child->IsSelfVisible()) {
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
    if (render_bitmap == nullptr) {
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

    Point position_in_parent = TranslateToParentPoint(repaint_rect.position);

    zaf::Rect repaint_rect_in_parent(position_in_parent, repaint_rect.size);
    repaint_rect_in_parent.Intersect(parent->ContentRect());
    parent->NeedRepaintRect(repaint_rect_in_parent);
}


void Control::RecalculateCachedPaintingRect(const zaf::Rect& repaint_rect) {

    if (cached_renderer_ == nullptr) {
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

    if (!is_layouting_.Value() || (auto_width_ || auto_height_)) {
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

        auto layout_guard = is_layouting_.BeginSet(true);
        Layout(previous_rect);
    }
}


zaf::Rect Control::AbsoluteRect() const {

    auto window = Window();
    if (window == nullptr) {
        return zaf::Rect();
    }

    //No parent, must be the root control, return its rect as the absolute rect.
    auto parent = Parent();
    if (parent == nullptr) {
        return Rect();
    }

    zaf::Rect parent_absolute_rect = parent->AbsoluteRect();
    const auto& parent_border = parent->Border();
    const auto& parent_padding = parent->Padding();

    return zaf::Rect(
        parent_absolute_rect.position.x + parent_border.left + parent_padding.left + rect_.position.x,
        parent_absolute_rect.position.y + parent_border.top + parent_padding.top + rect_.position.y,
        rect_.size.width,
        rect_.size.height
    );
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
            if (IsAncestorOf(focused_control)) {
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
}


Observable<RectChangedInfo> Control::RectChangedEvent() const {
    return rect_changed_event_.GetObservable();
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

        if (!each_child->IsSelfVisible()) {
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

    if (is_auto_resizing_.Value()) {
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
    auto auto_resize_guard = is_auto_resizing_.BeginSet(true);

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


zaf::Rect Control::AbsoluteContentRect() const {

    auto result = ContentRect();
    result.AddOffset(AbsoluteRect().position);
    return result;
}


zaf::Rect Control::ContentRect() const {

    zaf::Rect content_rect = zaf::Rect(Point(), Size());
    content_rect.Deflate(Border());
    content_rect.Deflate(Padding());
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

    bool need_release_renderer_resources = [&previous_parent, &parent]() {

        if (!previous_parent) {
            return false;
        }

        if (!parent) {
            return true;
        }

        auto previous_window = previous_parent->Window();
        if (!previous_window) {
            return false;
        }

        auto new_window = parent->Window();
        if (!new_window) {
            return true;
        }

        return previous_window != new_window;
    }();

    if (need_release_renderer_resources) {
        ReleaseRendererResources();
    }

    float old_dpi = GetDPI();
    parent_ = parent;

    ParentChangedInfo event_info{ shared_from_this(), previous_parent };
    OnParentChanged(event_info);

    if (old_dpi != GetDPI()) {
        OnDPIChanged();
    }
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
        previous_parent->RemoveChild(child);
    }

    child->SetParent(shared_from_this());
    children_.push_back(child);

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


void Control::RemoveChild(const std::shared_ptr<Control>& child) {

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
            if (child == focused_control || child->IsAncestorOf(focused_control)) {
                focused_control->SetIsFocused(false);
            }
        }
    }

    child->SetParent(nullptr);

    auto removed_iterator = std::find(children_.begin(), children_.end(), child);
    if (removed_iterator == children_.end()) {
        return;
    }

    auto removed_index = std::distance(children_.begin(), removed_iterator);
    children_.erase(removed_iterator);

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
            if (each_child == focused_control || each_child->IsAncestorOf(focused_control)) {
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

        auto recursive_child = child->InnerFindChildAtPosition(ToChildPoint(position, child), true);
        if (recursive_child) {
            return recursive_child;
        }

        return child;
    }

    return nullptr;
}


bool Control::IsParentOf(const std::shared_ptr<const Control>& child) const {
    return child->Parent().get() == this;
}


bool Control::IsAncestorOf(const std::shared_ptr<const Control>& child) const {

    auto ancestor = child->Parent();
    while (ancestor != nullptr) {

        if (ancestor.get() == this) {
            return true;
        }

        ancestor = ancestor->Parent();
    }

    return false;
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


bool Control::IsVisible() const {

    if (! is_visible_) {
        return false;
    }

    auto parent = Parent(); 
    if (parent == nullptr) {
        return true;
    }

    return parent->IsVisible();
}


bool Control::IsSelfVisible() const {
    return is_visible_;
}


void Control::SetIsVisible(bool is_visible) {

    bool need_relayout = is_visible_ != is_visible;

    SetInteractiveProperty(is_visible, is_visible_, &Control::OnIsVisibleChanged);

    //Notify parent to re-layout.
    if (need_relayout) {

        auto parent = Parent();
        if (parent) {

            parent->NeedRelayout();
            parent->AutoResizeToPreferredSize();
        }
    }
}


void Control::OnIsVisibleChanged() {

    for (const auto& each_child : children_) {
        each_child->OnIsVisibleChanged();
    }
}


bool Control::IsEnabled() const {

    if (! is_enabled_) {
        return false;
    }

    auto parent = Parent();
    if (parent == nullptr) {
        return true;    
    }

    return parent->IsEnabled();   
}


void Control::SetIsEnabled(bool is_enabled) {
    SetInteractiveProperty(is_enabled, is_enabled_, &Control::OnIsEnabledChanged);
}


void Control::OnIsEnabledChanged() {

    for (const auto& each_child : children_) {
        each_child->OnIsEnabledChanged();
    }
}


void Control::SetInteractiveProperty(bool new_value, bool& property_value, void(Control::*notification)()) {

    if (property_value == new_value) {
        return;
    }

    if (! new_value) {

        if (IsMouseOver()) {
            auto window = Window();
            if (window != nullptr) {
                window->SetMouseOverControl(nullptr, MouseMessage{ Message{} });
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


bool Control::IsSelected() const {
    
    if (is_selected_) {
        return true;
    }

    auto parent = Parent();
    if (!parent) {
        return false;
    }

    return parent->IsSelected();
}


void Control::SetIsSelected(bool is_selected) {

    if (is_selected_ == is_selected) {
        return;
    }

    is_selected_ = is_selected;
    NeedRepaint();

    OnIsSelectedChanged();
}


void Control::OnIsSelectedChanged() {

    for (const auto& each_child : children_) {
        each_child->OnIsSelectedChanged();
    }
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

    return IsAncestorOf(mouse_over_control);
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

    return IsAncestorOf(focused_control);
}


void Control::SetIsCachedPaintingEnabled(bool value) {

    if (is_cached_painting_enabled_ == value) {
        return;
    }

    is_cached_painting_enabled_ = value;
    ReleaseCachedPaintingRenderer();
}


void Control::ReleaseCachedPaintingRenderer() {
    cached_renderer_ = nullptr;
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
    Point absolute_position = AbsoluteRect().position;
    mouse_position.x -= absolute_position.x;
    mouse_position.y -= absolute_position.y;
    return mouse_position;
}


void Control::FindMouseOverControl(const Point& position, const MouseMessage& message) {

    auto window = Window();
    if (!window) {
        return;
    }

    auto found_control = FindEnabledControlAtPosition(position);
    if (found_control) {
        window->SetMouseOverControl(found_control, message);
    }
    else {
        window->SetMouseOverControl(nullptr, MouseMessage{ Message{} });
    }
}


std::shared_ptr<Control> Control::FindEnabledControlAtPosition(const Point& position) {

    //There is no enabled control if current control is disabled.
    if (!IsEnabled()) {
        return nullptr;
    }

    auto child = FindChildAtPosition(position);
    if (child && child->IsEnabled()) {
        return child->FindEnabledControlAtPosition(ToChildPoint(position, child));
    }

    return shared_from_this();
}


Point Control::TranslateToParentPoint(const Point& point) const {

    auto result = point;
    result.x += this->X(); 
    result.y += this->Y(); 

    auto parent = Parent();
    if (parent) {
        result.x += parent->Padding().Left() + parent->Border().Left();
        result.y += parent->Padding().Top() + parent->Border().Top();
    }

    return result;
}


Point Control::ToChildPoint(const Point& point, const std::shared_ptr<Control>& child) const {

    const auto& border = Border();
    const auto& padding = Padding();
    const auto& child_position = child->Position();

    Point point_in_child = point;
    point_in_child.x -= child_position.x + border.left + padding.left;
    point_in_child.y -= child_position.y + border.top + padding.top;

    return point_in_child;
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


void Control::OnMouseWheel(const MouseWheelInfo& event_info) {

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


void Control::OnKeyDown(const KeyDownInfo& event_info) {
    key_down_event_.Raise(event_info);
}


Observable<KeyDownInfo> Control::KeyDownEvent() const {
    return key_down_event_.GetObservable();
}


void Control::OnKeyUp(const KeyUpInfo& event_info) {
    key_up_event_.Raise(event_info);
}


Observable<KeyUpInfo> Control::KeyUpEvent() const {
    return key_up_event_.GetObservable();
}


void Control::OnCharInput(const CharInputInfo& event_info) {
    char_input_event_.Raise(event_info);
}


Observable<CharInputInfo> Control::CharInputEvent() const {
    return char_input_event_.GetObservable();
}


void Control::OnSysKeyDown(const SysKeyDownInfo& event_info) {
    sys_key_down_event_.Raise(event_info);
}


Observable<SysKeyDownInfo> Control::SysKeyDownEvent() const {
    return sys_key_down_event_.GetObservable();
}


void Control::OnSysKeyUp(const SysKeyUpInfo& event_info) {
    sys_key_up_event_.Raise(event_info);
}


Observable<SysKeyUpInfo> Control::SysKeyUpEvent() const {
    return sys_key_up_event_.GetObservable();
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

    auto window = Window();
    if (window) {
        return window->GetDPI();
    }
    return Application::Instance().GetSystemDPI();
}


void Control::OnDPIChanged() {

    for (const auto& each_child : children_) {
        each_child->OnDPIChanged();
    }
}


void Control::OnParentChanged(const ParentChangedInfo& event_info) {
    parent_changed_event_.Raise(event_info);
}


Observable<ParentChangedInfo> Control::ParentChangedEvent() const {
    return parent_changed_event_.GetObservable();
}


std::shared_ptr<internal::InspectorPort> Control::GetInspectorPort() const {

    auto window = Window();
    if (!window) {
        return nullptr;
    }

    return window->GetInspectorPort();
}

}