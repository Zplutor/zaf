#include <zaf/control/split_control.h>
#include <algorithm>
#include <zaf/base/log.h>
#include <zaf/creation.h>
#include <zaf/graphic/canvas.h>
#include <zaf/window/message/mouse_message.h>

#ifdef min
#undef min
#endif

namespace zaf {
namespace {
const wchar_t* const kIsHorizontalSplitPropertyName = L"IsHorizontalSplit";
const wchar_t* const kIsSplitBarDistanceFlippedPropertyName = L"IsSplitBarDistanceFlipped";
const wchar_t* const kMaximumSplitBarDistanceProprtyName = L"MaximumSplitBarDistance";
const wchar_t* const kMinimumSplitBarDistanceProprtyName = L"MinimumSplitBarDistance";
const wchar_t* const kSplitBarDistancePropertyName = L"SplitBarDistance";
const wchar_t* const kSplitBarThicknessPropertyName = L"SplitBarThickness";
const wchar_t* const kSplitterColorPickerPropertyName = L"SplitterColorPicker";
}

SplitControl::SplitControl() {

}


SplitControl::~SplitControl() {
    UninitializeSplitBar();
}


void SplitControl::Initialize() {

    __super::Initialize();

    split_bar_ = Create<SplitBar>();
    InitializeSplitBar();

    first_pane_ = Create<Control>();
    AddChild(first_pane_);

    second_pane_ = Create<Control>();
    AddChild(second_pane_);
}


void SplitControl::InitializeSplitBar() {

    split_bar_->SetIsHorizontal(IsHorizontalSplit());

    auto tag = reinterpret_cast<std::uintptr_t>(this);
    split_bar_->GetBeginDragEvent().AddListenerWithTag(tag, std::bind(&SplitControl::SplitBarBeginDrag, this));
    split_bar_->GetDragEvent().AddListenerWithTag(tag, std::bind(&SplitControl::SplitBarDrag, this));
    split_bar_->GetEndDragEvent().AddListenerWithTag(tag, std::bind(&SplitControl::SplitBarEndDrag, this));

    AddChild(split_bar_);
}


void SplitControl::UninitializeSplitBar() {

    auto tag = reinterpret_cast<std::uintptr_t>(this);
    split_bar_->GetBeginDragEvent().RemoveListenersWithTag(tag);
    split_bar_->GetDragEvent().RemoveListenersWithTag(tag);
    split_bar_->GetEndDragEvent().RemoveListenersWithTag(tag);

    RemoveChild(split_bar_);
}


void SplitControl::Layout(const Rect& previous_rect) {

    bool is_horizontal = IsHorizontalSplit();

    auto content_size = GetContentSize();
    float primary_length = content_size.width;
    float secondly_length = content_size.height;
    if (is_horizontal) {
        std::swap(primary_length, secondly_length);
    }

    float split_bar_thickness = GetSplitBarThickness();
    float split_bar_distance = GetUnflippedSplitBarDistance();

    Rect split_bar_rect(
        split_bar_distance, 
        0,
        split_bar_thickness, 
        secondly_length);
    
    Rect first_pane_rect(
        0, 
        0,
        split_bar_distance,
        secondly_length);

    Rect second_pane_rect(
        split_bar_distance + split_bar_thickness,
        0,
        primary_length - split_bar_distance - split_bar_thickness,
        secondly_length);

    if (is_horizontal) {
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


bool SplitControl::IsHorizontalSplit() const {

    auto is_horizontal = GetPropertyMap().TryGetProperty<bool>(kIsHorizontalSplitPropertyName);
    if (is_horizontal != nullptr) {
        return *is_horizontal;
    }
    else {
        return false;
    }
}

void SplitControl::SetIsHorizontalSplit(bool is_horizontal) {

    GetPropertyMap().SetProperty(kIsHorizontalSplitPropertyName, is_horizontal);
    split_bar_->SetIsHorizontal(is_horizontal);
    NeedRelayout();
}


float SplitControl::GetSplitBarThickness() const {

    auto thickness = GetPropertyMap().TryGetProperty<float>(kSplitBarThicknessPropertyName);
    if (thickness != nullptr) {
        return *thickness;
    }
    else {
        return 3;
    }
}

void SplitControl::SetSplitBarThickness(float thickness) {

    GetPropertyMap().SetProperty(kSplitBarThicknessPropertyName, thickness);
    NeedRelayout();
}


float SplitControl::GetSplitBarDistance() const {

    float distance = 0;

    auto expected_distance = GetPropertyMap().TryGetProperty<float>(kSplitBarDistancePropertyName);
    if (expected_distance != nullptr) {
        distance = *expected_distance;
    }
    else {
        auto content_size = GetContentSize();
        distance = (IsHorizontalSplit() ? content_size.height : content_size.width) / 2;
    }

    distance = std::max(distance, GetMinimumSplitBarDistance());
    distance = std::min(distance, GetMaximumSplitBarDistance());
    return distance;
}

void SplitControl::SetSplitBarDistance(float distance) {

    GetPropertyMap().SetProperty(kSplitBarDistancePropertyName, distance);
    NeedRelayout();
}


float SplitControl::GetMinimumSplitBarDistance() const {

    auto distance = GetPropertyMap().TryGetProperty<float>(kMinimumSplitBarDistanceProprtyName);
    if (distance != nullptr) {
        return std::max(*distance, 0.f);
    }
    else {
        return 0;
    }
}

void SplitControl::SetMinimumSplitBarDistance(float min_distance) {

    GetPropertyMap().SetProperty(kMinimumSplitBarDistanceProprtyName, min_distance);
    NeedRelayout();
}


float SplitControl::GetMaximumSplitBarDistance() const {

    auto content_size = GetContentSize();
    float avaliable_max_distance = (IsHorizontalSplit() ? content_size.height : content_size.width) - GetSplitBarThickness();
    if (avaliable_max_distance < 0) {
        return 0;
    }

    auto distance = GetPropertyMap().TryGetProperty<float>(kMaximumSplitBarDistanceProprtyName);
    if (distance != nullptr) {
        return std::min(*distance, avaliable_max_distance);
    }
    else {
        return avaliable_max_distance;
    }
}

void SplitControl::SetMaximumSplitBarDistance(float max_distance) {

    GetPropertyMap().SetProperty(kMaximumSplitBarDistanceProprtyName, max_distance);
    NeedRelayout();
}


bool SplitControl::IsSplitBarDistanceFlipped() const {

    auto is_flipped = GetPropertyMap().TryGetProperty<bool>(kIsSplitBarDistanceFlippedPropertyName);
    if (is_flipped != nullptr) {
        return *is_flipped;
    }

    return false;
}

void SplitControl::SetIsSplitBarDistanceFlipped(bool is_flipped) {

    GetPropertyMap().SetProperty(kIsSplitBarDistanceFlippedPropertyName, is_flipped);
    NeedRelayout();
}


float SplitControl::GetUnflippedSplitBarDistance() const {

    auto distance = GetSplitBarDistance();
    bool is_flipped = IsSplitBarDistanceFlipped();

    if (is_flipped) {
        auto content_size = GetContentSize();
        distance = (IsHorizontalSplit() ? content_size.height : content_size.width) - GetSplitBarThickness() - distance;
    }

    return distance;
}


void SplitControl::SetSplitBar(const std::shared_ptr<SplitBar>& split_bar) {

    if (split_bar_ == split_bar) {
        return;
    }

    auto previous_split_bar = split_bar;

    {
        UpdateGuard update_guard(*this);
        UninitializeSplitBar();
        split_bar_ = split_bar;
        InitializeSplitBar();
    }

    SplitBarChange(previous_split_bar);
}


void SplitControl::SetFirstPane(const std::shared_ptr<Control>& pane) {
    SetPane(first_pane_, pane, std::bind(&SplitControl::FirstPaneChange, this, std::placeholders::_1));
}


void SplitControl::SetSecondPane(const std::shared_ptr<Control>& pane) {
    SetPane(second_pane_, pane, std::bind(&SplitControl::SecondPaneChange, this, std::placeholders::_1));
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
        UpdateGuard update_guard(*this);
        RemoveChild(pane);
        pane = new_pane;
        AddChild(pane);
    }

    notification(previous_pane);
}


void SplitControl::SplitBarBeginDrag() {
    split_bar_begin_drag_mouse_position_ = GetSplitBarDragPosition();
    split_bar_begin_drag_distance_ = GetSplitBarDistance();
}


void SplitControl::SplitBarDrag() {

    float mouse_position = GetSplitBarDragPosition();
    float difference = mouse_position - split_bar_begin_drag_mouse_position_;
    if (IsSplitBarDistanceFlipped()) {
        difference *= -1;
    }
    float new_distance = split_bar_begin_drag_distance_ + difference;
    SetSplitBarDistance(new_distance); 
}


void SplitControl::SplitBarEndDrag() {
    split_bar_begin_drag_mouse_position_ = 0;
    split_bar_begin_drag_distance_ = 0;
}


float SplitControl::GetSplitBarDragPosition() const {

    auto mouse_position = GetMousePosition();
    return IsHorizontalSplit() ? mouse_position.y : mouse_position.x;
}


void SplitControl::SplitBar::Initialize() {

    __super::Initialize();
}


void SplitControl::SplitBar::Paint(Canvas& canvas, const Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    auto size = GetSize();
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

    Canvas::StateGuard state_guard(canvas);
    canvas.SetBrushWithColor(GetSplitterColor());
    canvas.DrawLine(start_point, end_point, 1);
}


ColorPicker SplitControl::SplitBar::GetSplitterColorPicker() const {

    auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(kSplitterColorPickerPropertyName);
    if ((color_picker != nullptr) && (*color_picker != nullptr)) {
        return *color_picker;
    }

    return [](const Control&) {
        return zaf::Color::Black;
    };
}


void SplitControl::SplitBar::SetSplitterColorPicker(const ColorPicker& color_picker) {
    GetPropertyMap().SetProperty(kSplitterColorPickerPropertyName, color_picker);
    NeedRepaint();
}


void SplitControl::SplitBar::ChangeMouseCursor(const Message& message, bool& is_changed) {

    SetCursor(LoadCursor(nullptr, IsHorizontal() ? IDC_SIZENS : IDC_SIZEWE));
    is_changed = true;
}


void SplitControl::SplitBar::MouseMove(const Point& position, const MouseMessage& message) {

    if (IsCapturingMouse()) {
        drag_event_.Trigger(std::dynamic_pointer_cast<SplitBar>(shared_from_this()));
    }
}


void SplitControl::SplitBar::MouseDown(const Point& position, const MouseMessage& message) {

    if (message.GetMouseButton() == MouseButton::Left) {
        CaptureMouse();
    }
}


void SplitControl::SplitBar::MouseUp(const Point& position, const MouseMessage& message) {

    if (IsCapturingMouse()) {
        ReleaseMouse();
    }
}


void SplitControl::SplitBar::MouseCapture() {

    begin_drag_event_.Trigger(std::dynamic_pointer_cast<SplitBar>(shared_from_this()));
}


void SplitControl::SplitBar::MouseRelease() {

    end_drag_event_.Trigger(std::dynamic_pointer_cast<SplitBar>(shared_from_this()));
}


}