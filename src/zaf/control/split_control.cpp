#include <zaf/control/split_control.h>
#include <algorithm>
#include <zaf/base/log.h>
#include <zaf/control/control_object.h>
#include <zaf/creation.h>
#include <zaf/graphic/canvas.h>
#include <zaf/window/message/mouse_message.h>

#undef min
#undef max

namespace zaf {

ZAF_OBJECT_IMPL(SplitControl);
ZAF_OBJECT_IMPL(SplitBar);


SplitControl::SplitControl() {

}


SplitControl::~SplitControl() {
    UninitializeSplitBar();
}


void SplitControl::Initialize() {

    __super::Initialize();

    split_bar_ = Create<zaf::SplitBar>();
    InitializeSplitBar();

    first_pane_ = Create<Control>();
    AddChild(first_pane_);

    second_pane_ = Create<Control>();
    AddChild(second_pane_);
}


void SplitControl::InitializeSplitBar() {

    split_bar_->SetIsHorizontal(IsHorizontalSplit());

    split_bar_subscriptions_ += split_bar_->BeginDragEvent().Subscribe(
        std::bind(&SplitControl::SplitBarBeginDrag, this));

    split_bar_subscriptions_ += split_bar_->DragEvent().Subscribe(
        std::bind(&SplitControl::SplitBarDrag, this));

    split_bar_subscriptions_ += split_bar_->EndDragEvent().Subscribe(
        std::bind(&SplitControl::SplitBarEndDrag, this));

    AddChild(split_bar_);
}


void SplitControl::UninitializeSplitBar() {

    if (split_bar_ == nullptr) {
        return;
    }

    split_bar_subscriptions_.Clear();

    RemoveChild(split_bar_);
}


void SplitControl::Layout(const zaf::Rect& previous_rect) {

    auto content_size = ContentSize();
    if (IsHorizontalSplit()) {
        std::swap(content_size.width, content_size.height);
    }

    zaf::Rect split_bar_rect(actual_split_distance_, 0, SplitBarThickness(), content_size.height);

    auto first_pane_margin = FirstPane()->Margin();
    if (IsHorizontalSplit()) {
        std::swap(first_pane_margin.left, first_pane_margin.top);
        std::swap(first_pane_margin.right, first_pane_margin.bottom);
    }

    zaf::Rect first_pane_rect(
        first_pane_margin.left,
        first_pane_margin.top,
        actual_split_distance_ - first_pane_margin.Width(),
        content_size.height - first_pane_margin.Height());
    
    auto second_pane_margin = SecondPane()->Margin();
    if (IsHorizontalSplit()) {
        std::swap(second_pane_margin.left, second_pane_margin.top);
        std::swap(second_pane_margin.right, second_pane_margin.bottom);
    }

    zaf::Rect second_pane_rect(
        split_bar_rect.Right() + second_pane_margin.left,
        second_pane_margin.top,
        content_size.width - split_bar_rect.Right() - second_pane_margin.Width(),
        content_size.height - second_pane_margin.Height());

    if (IsHorizontalSplit()) {
        std::swap(split_bar_rect.position.x, split_bar_rect.position.y);
        std::swap(split_bar_rect.size.width, split_bar_rect.size.height);
        std::swap(first_pane_rect.position.x, first_pane_rect.position.y);
        std::swap(first_pane_rect.size.width, first_pane_rect.size.height);
        std::swap(second_pane_rect.position.x, second_pane_rect.position.y);
        std::swap(second_pane_rect.size.width, second_pane_rect.size.height);
    }

    split_bar_->SetRect(split_bar_rect);
    first_pane_->SetRect(first_pane_rect);
    second_pane_->SetRect(second_pane_rect);
}


void SplitControl::OnRectChanged(const RectChangedInfo& event_info) {

    __super::OnRectChanged(event_info);

    if (IsHorizontalSplit()) {
        if (event_info.PreviousRect().size.height == Height()) {
            return;
        }
    }
    else {
        if (event_info.PreviousRect().size.width == Width()) {
            return;
        }
    }

    if (UpdateActualSplitDistance(false)) {
        NeedRelayout();
    }
}


bool SplitControl::UpdateActualSplitDistance(bool is_by_dragging) {

    float total_length{};
    float min_distance{};
    float max_distance{};
    GetSplitDistanceLimit(total_length, min_distance, max_distance);

    float distance = 0;
    if (expected_split_distance_.has_value()) {
        distance = *expected_split_distance_;
    }
    else {
        distance = (total_length - SplitBarThickness()) / 2;
    }

    distance = std::max(distance, min_distance);
    distance = std::min(distance, max_distance);

    if (distance == actual_split_distance_) {
        return false;
    }

    auto previous_distance = actual_split_distance_;
    actual_split_distance_ = distance;

    //Raise the event.
    split_distance_changed_event_.Raise(SplitDistanceChangedInfo{
        As<SplitControl>(shared_from_this()),
        previous_distance,
        is_by_dragging
    });

    return true;
}


void SplitControl::GetSplitDistanceLimit(float& total_length, float& min, float& max) const {

    auto content_size = ContentSize();
    total_length = IsHorizontalSplit() ? content_size.height : content_size.width;

    //Revise min max within total length.
    float first_pane_max = 
        std::min(first_pane_max_length_, std::max(total_length - SplitBarThickness(), 0.f));

    float second_pane_flipped_max = 
        std::max(total_length - SplitBarThickness() - second_pane_min_length_, 0.f);

    float second_pane_flipped_min = 
        std::max(total_length - SplitBarThickness() - second_pane_max_length_, 0.f);

    min = std::max(first_pane_min_length_, second_pane_flipped_min);
    max = std::min(first_pane_max, second_pane_flipped_max);
}


void SplitControl::SetIsHorizontalSplit(bool is_horizontal) {

    if (is_horizontal_split_ == is_horizontal) {
        return;
    }

    is_horizontal_split_ = is_horizontal;
    split_bar_->SetIsHorizontal(is_horizontal);

    UpdateActualSplitDistance(false);
    NeedRelayout();
}


void SplitControl::SetSplitBarThickness(float thickness) {

    if (split_bar_thickness_ == thickness) {
        return;
    }

    split_bar_thickness_ = thickness;
    UpdateActualSplitDistance(false);
    NeedRelayout();
}


void SplitControl::SetSplitDistance(float expected_distance) {
    InnerSetSplitDistance(expected_distance, false);
}


void SplitControl::InnerSetSplitDistance(float distance, bool is_by_dragging) {

    if (expected_split_distance_.has_value() &&
        expected_split_distance_.value() == distance) {
        return;
    }

    expected_split_distance_ = distance;

    if (UpdateActualSplitDistance(is_by_dragging)) {
        NeedRelayout();
    }
}


Observable<SplitDistanceChangedInfo> SplitControl::SplitDistanceChangedEvent() const {
    return split_distance_changed_event_.GetObservable();
}


void SplitControl::SetSplitBar(const std::shared_ptr<zaf::SplitBar>& split_bar) {

    if (split_bar_ == split_bar) {
        return;
    }

    auto previous_split_bar = split_bar;

    {
        auto update_guard = this->BeginUpdate();
        UninitializeSplitBar();
        split_bar_ = split_bar;
        InitializeSplitBar();
    }

    OnSplitBarChanged(previous_split_bar);
}


void SplitControl::SetFirstPane(const std::shared_ptr<Control>& pane) {
    SetPane(
        first_pane_, 
        pane, 
        std::bind(&SplitControl::OnFirstPaneChanged, this, std::placeholders::_1));
}


void SplitControl::SetSecondPane(const std::shared_ptr<Control>& pane) {
    SetPane(
        second_pane_, 
        pane, 
        std::bind(&SplitControl::OnSecondPaneChanged, this, std::placeholders::_1));
}


void SplitControl::SetPane(
    std::shared_ptr<Control>& pane,
    const std::shared_ptr<Control>& new_pane,
    const std::function<void(const std::shared_ptr<Control>&)>& notification) {

    if (pane == new_pane) {
        return;
    }

    auto previous_pane = pane;

    {
        auto update_guard = this->BeginUpdate();
        RemoveChild(pane);
        pane = new_pane;
        AddChild(pane);
    }

    notification(previous_pane);
}


void SplitControl::SetFirstPaneMinLength(float length) {
    SetPaneLimitLength(length, true, first_pane_min_length_, first_pane_max_length_);
}


void SplitControl::SetFirstPaneMaxLength(float length) {
    SetPaneLimitLength(length, false, first_pane_min_length_, first_pane_max_length_);
}


void SplitControl::SetSecondPaneMinLength(float length) {
    SetPaneLimitLength(length, true, second_pane_min_length_, second_pane_max_length_);
}


void SplitControl::SetSecondPaneMaxLength(float length) {
    SetPaneLimitLength(length, false, second_pane_min_length_, second_pane_max_length_);
}


void SplitControl::SetPaneLimitLength(
    float new_length, 
    bool is_setting_min, 
    float& min,
    float& max) {

    float& setting_value = is_setting_min ? min : max;
    if (setting_value == new_length) {
        return;
    }

    setting_value = new_length;

    float& other_value = is_setting_min ? max : min;
    if (min > max) {
        other_value = setting_value;
    }

    if (UpdateActualSplitDistance(false)) {
        NeedRelayout();
    }
}


void SplitControl::SplitBarBeginDrag() {
    split_bar_begin_drag_mouse_position_ = GetSplitBarDragPosition();
    split_bar_begin_drag_distance_ = SplitDistance();
}


void SplitControl::SplitBarDrag() {

    float mouse_position = GetSplitBarDragPosition();
    float difference = mouse_position - split_bar_begin_drag_mouse_position_;
    float new_distance = split_bar_begin_drag_distance_ + difference;
    InnerSetSplitDistance(new_distance, true); 
}


void SplitControl::SplitBarEndDrag() {
    split_bar_begin_drag_mouse_position_ = 0;
    split_bar_begin_drag_distance_ = 0;
}


float SplitControl::GetSplitBarDragPosition() const {

    auto mouse_position = GetMousePosition();
    return IsHorizontalSplit() ? mouse_position.y : mouse_position.x;
}


SplitBar::SplitBar() {

}


void SplitBar::Initialize() {

    __super::Initialize();

    SetSplitterColor(Color::Black());
}


void SplitBar::UpdateStyle() {

    __super::UpdateStyle();

    splitter_color_field_.UpdateColor(*this);
}


void SplitBar::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const {

    __super::Paint(canvas, dirty_rect);

    auto size = Size();
    float primary_length = size.height;
    float secondly_length = size.width;
    if (IsHorizontal()) {
        std::swap(primary_length, secondly_length);
    }

    Point start_point(secondly_length / 2, 0);
    Point end_point(secondly_length / 2, primary_length);

    if (IsHorizontal()) {
        std::swap(start_point.x, start_point.y);
        std::swap(end_point.x, end_point.y);
    }

    auto state_guard = canvas.PushState();
    canvas.SetBrushWithColor(SplitterColor());
    canvas.DrawLine(start_point, end_point, 1);
}


Color SplitBar::SplitterColor() const {
    return splitter_color_field_.Color();
}

void SplitBar::SetSplitterColor(const Color& color) {
    splitter_color_field_.SetColor(color, *this);
}


const ColorPicker& SplitBar::SplitterColorPicker() const {
    return splitter_color_field_.ColorPicker();
}

void SplitBar::SetSplitterColorPicker(ColorPicker picker) {
    splitter_color_field_.SetColorPicker(std::move(picker), *this);
}


void SplitBar::OnMouseCursorChanging(const MouseCursorChangingInfo& event_info) {

    __super::OnMouseCursorChanging(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    SetCursor(LoadCursor(nullptr, IsHorizontal() ? IDC_SIZENS : IDC_SIZEWE));
    event_info.MarkAsHandled();
}


void SplitBar::OnMouseMove(const MouseMoveInfo& event_info) {

    __super::OnMouseMove(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (IsCapturingMouse()) {

        SplitBarDragInfo drag_event_info{ As<SplitBar>(shared_from_this()) };
        drag_event_.AsObserver().OnNext(drag_event_info);

        event_info.MarkAsHandled();
    }
}


void SplitBar::OnMouseDown(const MouseDownInfo& event_info) {

    __super::OnMouseDown(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (event_info.Message().MouseButton() == MouseButton::Left) {

        CaptureMouse();
        event_info.MarkAsHandled();
    }
}


void SplitBar::OnMouseUp(const MouseUpInfo& event_info) {

    __super::OnMouseUp(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (IsCapturingMouse()) {

        ReleaseMouse();
        event_info.MarkAsHandled();
    }
}


void SplitBar::OnMouseCaptured(const MouseCapturedInfo& event_info) {

    __super::OnMouseCaptured(event_info);

    SplitBarBeginDragInfo begin_drag_event_info{ As<SplitBar>(shared_from_this()) };
    begin_drag_event_.AsObserver().OnNext(begin_drag_event_info);
}


void SplitBar::OnMouseReleased(const MouseReleasedInfo& event_info) {

    __super::OnMouseReleased(event_info);

    SplitBarEndDragInfo end_drag_event_info{ As<SplitBar>(shared_from_this()) };
    end_drag_event_.AsObserver().OnNext(end_drag_event_info);
}


}