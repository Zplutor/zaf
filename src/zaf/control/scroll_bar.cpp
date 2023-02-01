#include <zaf/control/scroll_bar.h>
#include <zaf/base/error/check.h>
#include <zaf/base/event_utility.h>
#include <zaf/base/timer.h>
#include <zaf/control/scroll_bar_arrow.h>
#include <zaf/control/scroll_bar_thumb.h>
#include <zaf/creation.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/geometry/geometry_sink.h>
#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/geometry/transformed_geometry.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/internal/theme.h>
#include <zaf/object/type_definition.h>
#include <zaf/serialization/properties.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {
namespace {

constexpr const wchar_t* const kScrollEventPropertyName = L"ScrollEvent";

constexpr int kTimerInitialInterval = 300;
constexpr int kTimerContinuousInterval = 50;

}

ZAF_DEFINE_TYPE(ScrollBar)
ZAF_DEFINE_TYPE_PROPERTY_DYNAMIC(IncrementalArrow)
ZAF_DEFINE_TYPE_PROPERTY_DYNAMIC(DecrementalArrow)
ZAF_DEFINE_TYPE_PROPERTY_DYNAMIC(Thumb)
ZAF_DEFINE_TYPE_PROPERTY(IsHorizontal)
ZAF_DEFINE_TYPE_PROPERTY(ArrowLength)
ZAF_DEFINE_TYPE_PROPERTY(Value)
ZAF_DEFINE_TYPE_PROPERTY(MinValue)
ZAF_DEFINE_TYPE_PROPERTY(MaxValue)
ZAF_DEFINE_TYPE_PROPERTY(SmallChange)
ZAF_DEFINE_TYPE_PROPERTY(LargeChange)
ZAF_DEFINE_TYPE_PROPERTY(PageSize)
ZAF_DEFINE_TYPE_END

ScrollBar::ScrollBar() : 
    incremental_arrow_(Create<ScrollBarArrow>()),
    decremental_arrow_(Create<ScrollBarArrow>()),
    thumb_(Create<ScrollBarThumb>()),
    is_horizontal_(false),
    min_value_(0), 
    max_value_(0), 
    value_(0),
    begin_drag_value_(0),
    timer_event_(TimerEvent::Increment),
    remaining_wheel_change_value_(0) {

}


void ScrollBar::Initialize() {

    __super::Initialize();

    InitializeArrow(incremental_arrow_, true);
    InitializeArrow(decremental_arrow_, false);
    InitializeThumb(thumb_);
    ApplyOrientationToChildren();
}


void ScrollBar::InitializeArrow(const std::shared_ptr<ScrollBarArrow>& arrow, bool is_incremental) {

    auto& subscription_holder = 
        is_incremental ? 
        incremental_arrow_subscriptions_ : 
        decremental_arrow_subscriptions_;

    subscription_holder += arrow->BeginPressEvent().Subscribe(
        std::bind(&ScrollBar::ArrowBeginPress, this, std::placeholders::_1));

    subscription_holder += arrow->EndPressEvent().Subscribe(
        std::bind(&ScrollBar::ArrowEndPress, this, std::placeholders::_1));

    AddChild(arrow);
}


void ScrollBar::UninitializeArrow(
    const std::shared_ptr<ScrollBarArrow>& arrow, 
    bool is_incremental) {

    auto& subscription_holder =
        is_incremental ?
        incremental_arrow_subscriptions_ :
        decremental_arrow_subscriptions_;

    subscription_holder.Clear();

    RemoveChild(arrow);
}


void ScrollBar::InitializeThumb(const std::shared_ptr<ScrollBarThumb>& thumb) {

    thumb_subscriptions_ += thumb->BeginDragEvent().Subscribe(
        std::bind(&ScrollBar::ThumbBeginDrag, this, std::placeholders::_1));

    thumb_subscriptions_ += thumb->DragEvent().Subscribe(
        std::bind(&ScrollBar::ThumbDrag, this, std::placeholders::_1));

    thumb_subscriptions_ += thumb->EndDragEvent().Subscribe(
        std::bind(&ScrollBar::ThumbEndDrag, this, std::placeholders::_1));

    AddChild(thumb);
}


void ScrollBar::UninitializeThumb(const std::shared_ptr<ScrollBarThumb>& thumb) {

    thumb_subscriptions_.Clear();
    RemoveChild(thumb);
}


void ScrollBar::ApplyOrientationToChildren() {

    incremental_arrow_->SetDirection(
        is_horizontal_ ? ArrowDirection::Right : ArrowDirection::Down
    );
    decremental_arrow_->SetDirection(
        is_horizontal_ ? ArrowDirection::Left : ArrowDirection::Up
    );
    thumb_->SetIsHorizontal(is_horizontal_);
}


void ScrollBar::SetIncrementalArrow(const std::shared_ptr<ScrollBarArrow>& incremental_arrow) {

    auto previous_arrow = incremental_arrow_;
    incremental_arrow_ = incremental_arrow;

    if (IsParentOf(previous_arrow)) {
        UninitializeArrow(previous_arrow, true);
        InitializeArrow(incremental_arrow_, true);
        ApplyOrientationToChildren();
    }
}


void ScrollBar::SetDecrementalArrow(const std::shared_ptr<ScrollBarArrow>& decremental_arrow) {

    auto previous_arrow = decremental_arrow_;
    decremental_arrow_ = decremental_arrow;

    if (IsParentOf(previous_arrow)) {
        UninitializeArrow(previous_arrow, false);
        InitializeArrow(decremental_arrow_, false);
        ApplyOrientationToChildren();
    }
}


void ScrollBar::SetThumb(const std::shared_ptr<ScrollBarThumb>& thumb) {

    auto previous_thumb = thumb_;
    thumb_ = thumb;

    if (IsParentOf(previous_thumb)) {
        UninitializeThumb(previous_thumb);
        InitializeThumb(thumb_);
        ApplyOrientationToChildren();
    }
}


float ScrollBar::ArrowLength() const {

    auto length = GetPropertyMap().TryGetProperty<float>(property::ArrowLength);
    if (length != nullptr) {
        return *length;
    }
        
    if (IsHorizontal()) {
        return Height();
    }
    else {
        return Width();
    }
}


void ScrollBar::SetArrowLength(float length) {

    GetPropertyMap().SetProperty(property::ArrowLength, length);
    NeedRelayout();
}


void ScrollBar::SetValue(int value) {

    int previous_value = value_;
    value_ = value;
    
    if (value_ < min_value_) {
        value_ = min_value_;
    }
    else if (value_ > max_value_) {
        value_ = max_value_;
    }

    if (previous_value != value_) {

        NeedRelayout();

        auto event_observer = GetEventObserver<ScrollBarScrollInfo>(
            GetPropertyMap(), 
            kScrollEventPropertyName);

        if (event_observer) {
            ScrollBarScrollInfo event_info(
                std::dynamic_pointer_cast<ScrollBar>(shared_from_this()));
            event_observer->OnNext(event_info);
        }
    }
}


void ScrollBar::ChangeValueRange(int min_value, int max_value, bool max_value_has_priority) {

    min_value_ = min_value;
    max_value_ = max_value;

    if (min_value_ > max_value_) {

        if (max_value_has_priority) {
            min_value_ = max_value_;
        }
        else {
            max_value_ = min_value_;
        }
    }

    auto update_guard = this->BeginUpdate();
    SetValue(value_);
    NeedRelayout();
}


int ScrollBar::SmallChange() const {

    auto value = GetPropertyMap().TryGetProperty<int>(property::SmallChange);
    if (value != nullptr) {
        return *value;
    }
    else {
        return 1;
    }
}

void ScrollBar::SetSmallChange(int value) {
    GetPropertyMap().SetProperty(property::SmallChange, value);
}


int ScrollBar::LargeChange() const {

    auto value = GetPropertyMap().TryGetProperty<int>(property::LargeChange);
    if (value) {
        return *value;
    }
    else {
        return SmallChange();
    }
}

void ScrollBar::SetLargeChange(int value) {
    GetPropertyMap().SetProperty(property::LargeChange, value);
}


void ScrollBar::SetPageSize(int value) {

    ZAF_EXPECT(value >= 0);

    page_size_ = value;
    NeedRelayout();
}


void ScrollBar::Wheel(int distance) {

    double change_count = static_cast<double>(distance) / WHEEL_DELTA;
    int change_value = 0;

    UINT multiple = 1;
    SystemParametersInfo(
        IsHorizontal() ? SPI_GETWHEELSCROLLCHARS : SPI_GETWHEELSCROLLLINES,
        0,
        &multiple,
        0);

    //-1 means scroll a whole screen. But we don't know how many values per screen
    //here, so use large change value instead.
    if (multiple == -1) {
        change_value = LargeChange();
        multiple = 1;
    }
    else {
        change_value = SmallChange();
    }
    
    double expected_change_value = change_count * change_value * multiple;
    expected_change_value += remaining_wheel_change_value_;

    int actual_change_value = static_cast<int>(expected_change_value);
    remaining_wheel_change_value_ = expected_change_value - actual_change_value;
    
    int direction = IsHorizontal() ? 1 : -1;
    SetValue(value_ + actual_change_value * direction);
}


void ScrollBar::Layout(const zaf::Rect& previous_rect) {

    const zaf::Rect& rect = Rect();

    float bar_thickness = 0;
    float bar_length = 0;

    if (is_horizontal_) {
        bar_thickness = rect.size.height;
        bar_length = rect.size.width;
    }
    else {
        bar_thickness = rect.size.width;
        bar_length = rect.size.height;
    }

    float arrow_length = ArrowLength();
    zaf::Size arrow_size(bar_thickness, arrow_length);
    zaf::Rect decremental_arrow_rect(zaf::Rect(Point(), arrow_size));
    zaf::Rect incremental_arrow_rect(zaf::Rect(Point(0, bar_length - arrow_length), arrow_size));

    float thumb_position = 0;
    float thumb_length = 0;
    CalculateThumbPositionAndLength(bar_length - arrow_length * 2, thumb_position, thumb_length);

    zaf::Rect thumb_rect(0, thumb_position + arrow_length, bar_thickness, thumb_length);

    if (is_horizontal_) {
        ChangeVerticalRectToHorizontalRect(incremental_arrow_rect);
        ChangeVerticalRectToHorizontalRect(decremental_arrow_rect);
        ChangeVerticalRectToHorizontalRect(thumb_rect);
    }

    incremental_arrow_->SetRect(incremental_arrow_rect);
    decremental_arrow_->SetRect(decremental_arrow_rect);
    thumb_->SetRect(thumb_rect);
}


void ScrollBar::CalculateThumbPositionAndLength(
    float track_length, 
    float& thumb_position, 
    float& thumb_length) {

    const float min_length = 15;

    //Don't display the thumb if there is no enough space.
    if (track_length < min_length) {
        thumb_position = 0;
        thumb_length = 0;
        return;
    }

    thumb_length = CalculateThumbLength(track_length);
    if (thumb_length < min_length) {
        thumb_length = min_length;
    }

    thumb_position = CalculateThumbPosition(track_length, thumb_length);
}


float ScrollBar::CalculateThumbLength(float track_length) {

    int page_size = PageSize();

    int total_size = max_value_ - min_value_ + page_size;
    if (total_size == 0) {
        return 0;
    }

    auto page_percent = double(page_size) / total_size;
    return static_cast<float>(track_length * page_percent);
}


float ScrollBar::CalculateThumbPosition(float track_length, float thumb_length) {

    int value_range = max_value_ - min_value_;
    if (value_range == 0) {
        return 0;
    }

    int position = value_ - min_value_;
    auto position_percent = static_cast<double>(position) / value_range;
    return static_cast<float>((track_length - thumb_length) * position_percent);
}


void ScrollBar::ChangeVerticalRectToHorizontalRect(zaf::Rect& rect) {

    std::swap(rect.position.x, rect.position.y);
    std::swap(rect.size.width, rect.size.height);
}


Observable<ScrollBarScrollInfo> ScrollBar::ScrollEvent() {
    return GetEventObservable<ScrollBarScrollInfo>(GetPropertyMap(), kScrollEventPropertyName);
}


void ScrollBar::OnMouseDown(const MouseDownInfo& event_info) {

    __super::OnMouseDown(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (event_info.Message().MouseButton() == MouseButton::Left) {

        CaptureMouse();
        event_info.MarkAsHandled();
    }
}


void ScrollBar::OnMouseUp(const MouseUpInfo& event_info) {

    __super::OnMouseUp(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (event_info.Message().MouseButton() == MouseButton::Left) {

        ReleaseMouse();
        event_info.MarkAsHandled();
    }
}


void ScrollBar::OnMouseCapture() {
    BeginTimer(TimerEvent::PageRoll);
}


void ScrollBar::OnMouseRelease() {
    timer_.reset();
}


void ScrollBar::BeginTimer(TimerEvent timer_event) {

    timer_event_ = timer_event;
    ApplyTimerEvent();

    timer_ = std::make_shared<Timer>(Timer::Mode::OneShot);
    timer_->SetInterval(std::chrono::milliseconds(kTimerInitialInterval));
    Subscriptions() += timer_->TriggerEvent().Subscribe(std::bind(&ScrollBar::TimerTrigger, this));
    timer_->Start();
}


void ScrollBar::TimerTrigger() {

    ApplyTimerEvent();

    if (timer_->GetInterval() == std::chrono::milliseconds(kTimerInitialInterval)) {

        timer_ = std::make_shared<Timer>(Timer::Mode::DeferredRepeated);
        timer_->SetInterval(std::chrono::milliseconds(kTimerContinuousInterval));
        Subscriptions() += timer_->TriggerEvent().Subscribe(
            std::bind(&ScrollBar::TimerTrigger, this));
        timer_->Start();
    }
}


void ScrollBar::ApplyTimerEvent() {

    if (timer_event_ == TimerEvent::Increment) {

        if (incremental_arrow_->IsPressed()) {
            SetValue(value_ + SmallChange());
        }
    }
    else if (timer_event_ == TimerEvent::Decrement) {

        if (decremental_arrow_->IsPressed()) {
            SetValue(value_ - SmallChange());
        }
    }
    else if (timer_event_ == TimerEvent::PageRoll) {

        Point mouse_position = GetMousePosition();

        zaf::Rect thumb_slot_rect = GetThumbSlotRect();
        if (! thumb_slot_rect.Contain(mouse_position)) {
            return;
        }

        zaf::Rect thumb_rect = thumb_->Rect();
        if (thumb_rect.Contain(mouse_position)) {
            return;
        }

        bool is_page_decrement = false;

        if (is_horizontal_) {
            if (mouse_position.x < thumb_rect.position.x) {
                is_page_decrement = true;
            }
        }
        else {
            
            if (mouse_position.y < thumb_rect.position.y) {
                is_page_decrement = true;
            }
        }

        if (is_page_decrement) {
            SetValue(value_ - LargeChange());
        }
        else {
            SetValue(value_ + LargeChange());
        }
    }
}


zaf::Rect ScrollBar::GetThumbSlotRect() const {

    zaf::Rect thumb_slot_rect(Point(), Size());

    if (is_horizontal_) {

        float arrow_thickness = incremental_arrow_->Size().width;
        thumb_slot_rect.position.x += arrow_thickness;
        thumb_slot_rect.size.width -= arrow_thickness;
    }
    else {

        float arrow_thickness = incremental_arrow_->Size().height;
        thumb_slot_rect.position.y += arrow_thickness;
        thumb_slot_rect.size.height -= arrow_thickness;
    }

    return thumb_slot_rect;
}


void ScrollBar::OnMouseWheel(const MouseWheelInfo& event_info) {

    __super::OnMouseWheel(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (event_info.Message().IsHorizontalWheeling() == IsHorizontal()) {

        Wheel(event_info.Message().WheelingDistance());
        event_info.MarkAsHandled();
    }
}


void ScrollBar::ArrowBeginPress(const ScrollBarArrowBeginPressInfo& event_info) {

    if (event_info.Source() == incremental_arrow_) {
        BeginTimer(TimerEvent::Increment);
    }
    else if (event_info.Source() == decremental_arrow_) {
        BeginTimer(TimerEvent::Decrement);
    }
}


void ScrollBar::ArrowEndPress(const ScrollBarArrowEndPressInfo& event_info) {
    timer_.reset();
}


void ScrollBar::ThumbBeginDrag(const ScrollBarThumbBeginDragInfo& event_info) {

    begin_drag_value_ = value_;
    begin_drag_mouse_position_ = GetMousePosition();
}


void ScrollBar::ThumbDrag(const ScrollBarThumbDragInfo& event_info) {

    float begin_drag_mouse_position_value = begin_drag_mouse_position_.x;
    float begin_drag_mouse_position_bias = begin_drag_mouse_position_.y;

    Point mouse_position = GetMousePosition();
    float mouse_position_value = mouse_position.x;
    float mouse_position_bias = mouse_position.y;

    if (! is_horizontal_) {
        std::swap(begin_drag_mouse_position_value, begin_drag_mouse_position_bias);
        std::swap(mouse_position_value, mouse_position_bias);
    }

    float bias_offset = mouse_position_bias - begin_drag_mouse_position_bias;
    if (std::abs(bias_offset) > 150) {
        SetValue(begin_drag_value_);
    }
    else {

        float mouse_offset = mouse_position_value - begin_drag_mouse_position_value;
        float value_per_point = GetValuePerThumbSlotPoint();
        SetValue(static_cast<int>(begin_drag_value_ + mouse_offset * value_per_point));
    }
}


void ScrollBar::ThumbEndDrag(const ScrollBarThumbEndDragInfo& event_info) {

}


float ScrollBar::GetValuePerThumbSlotPoint() {

    const zaf::Size& bar_size = Size();
    const zaf::Size& thumb_size = thumb_->Size();

    float thumb_scroll_length = 0;
    if (is_horizontal_) {
        thumb_scroll_length = bar_size.width - thumb_size.width;
    }
    else {
        thumb_scroll_length = bar_size.height - thumb_size.height;
    }
    thumb_scroll_length -= ArrowLength() * 2;

    int value_range = max_value_ - min_value_;
    return value_range / thumb_scroll_length;
}

}