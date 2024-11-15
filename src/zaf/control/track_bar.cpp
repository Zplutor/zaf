#include <zaf/control/track_bar.h>
#include <zaf/graphic/canvas.h>
#include <zaf/rx/timer.h>

namespace zaf {

ZAF_OBJECT_IMPL(TrackBar);

void TrackBar::Initialize() {

    __super::Initialize();

    SetCanFocus(true);
    SetTrackColor(Color::Gray());
    SetThumb(Create<TrackBarThumb>());
}


void TrackBar::Layout(const zaf::Rect& old_rect) {

    __super::Layout(old_rect);

    auto content_size = ContentSize();

    float track_length = content_size.width;
    float thumb_length = content_size.height;
    if (!is_horizontal_) {
        std::swap(track_length, thumb_length);
    }

    zaf::Rect thumb_rect;
    int value_range = max_value_ - min_value_;
    if (value_range != 0) {
        auto point_per_value = (track_length - thumb_thickness_) / value_range;
        thumb_rect.position.x = point_per_value * value_;
    }

    thumb_rect.size.width = thumb_thickness_;
    thumb_rect.size.height = thumb_length;

    if (!is_horizontal_) {
        std::swap(thumb_rect.position.x, thumb_rect.position.y);
        std::swap(thumb_rect.size.width, thumb_rect.size.height);
    }

    thumb_->SetRect(thumb_rect);
}


void TrackBar::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const {

    __super::Paint(canvas, dirty_rect);

    auto content_rect = ContentRect();
    auto region_guard = canvas.PushRegion(content_rect, content_rect);

    float track_length = content_rect.size.width;
    float thumb_length = content_rect.size.height;
    if (!is_horizontal_) {
        std::swap(track_length, thumb_length);
    }

    Point from_position{ 0, thumb_length / 2 };
    Point end_position{ track_length, thumb_length / 2 };

    if (!is_horizontal_) {
        std::swap(from_position.x, from_position.y);
        std::swap(end_position.x, end_position.y);
    }

    auto state_guard = canvas.PushState();
    canvas.SetBrushWithColor(TrackColor());
    canvas.DrawLine(from_position, end_position, track_thickness_);
}


void TrackBar::OnMouseDown(const MouseDownInfo& event_info) {

    __super::OnMouseDown(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    if (event_info.Message().MouseButton() != MouseButton::Left) {
        return;
    }

    SetIsFocused(true);
    CaptureMouse();

    StartPressing();

    event_info.MarkAsHandled();
}


void TrackBar::OnMouseUp(const MouseUpInfo& event_info) {

    __super::OnMouseUp(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    if (event_info.Message().MouseButton() != MouseButton::Left) {
        return;
    }

    if (!pressing_zone_) {
        return;
    }

    pressing_zone_.reset();
    timer_sub_.Unsubscribe();

    if (IsCapturingMouse()) {
        ReleaseMouse();
    }

    event_info.MarkAsHandled();
}


void TrackBar::StartPressing() {

    pressing_zone_ = DetermineMouseZone();
    ChangeValueByPressingZone();
    StartPressingTimer(true);
}


void TrackBar::ContinuePressing() {

    if (!pressing_zone_) {
        return;
    }

    auto current_zone = DetermineMouseZone();
    if (current_zone == pressing_zone_) {
        ChangeValueByPressingZone();
    }

    StartPressingTimer(false);
}


TrackBar::Zone TrackBar::DetermineMouseZone() const {

    auto thumb_rect = thumb_->Rect();
    thumb_rect.AddOffset(this->ContentRect().position);

    float thumb_leading_position = is_horizontal_ ? thumb_rect.Left() : thumb_rect.Top();
    float thumb_tailing_position = is_horizontal_ ? thumb_rect.Right() : thumb_rect.Bottom();

    auto mouse_point = GetMousePosition();
    float mouse_position = is_horizontal_ ? mouse_point.x : mouse_point.y;

    if (mouse_position < thumb_leading_position) {
        return Zone::Leading;
    }
    else if (mouse_position >= thumb_tailing_position) {
        return Zone::Tailing;
    }
    else {
        return Zone::Thumb;
    }
}


void TrackBar::ChangeValueByPressingZone() {

    if (pressing_zone_ == Zone::Leading) {
        SetValue(value_ - large_change_);
    }
    else if (pressing_zone_ == Zone::Tailing) {
        SetValue(value_ + large_change_);
    }
}


void TrackBar::StartPressingTimer(bool is_initial) {

    auto timeout = is_initial ? std::chrono::milliseconds(300) : std::chrono::milliseconds(50);

    timer_sub_ = rx::Timer(timeout).Subscribe([this](int) {
        ContinuePressing();
    });
}


const std::shared_ptr<TrackBarThumb>& TrackBar::Thumb() const noexcept {
    return thumb_;
}

void TrackBar::SetThumb(std::shared_ptr<TrackBarThumb> thumb) {

    ZAF_EXPECT(thumb);

    if (thumb_) {
        RemoveChild(thumb_);
    }

    thumb_ = std::move(thumb);
    AddChild(thumb_);

    thumb_subs_.Clear();

    thumb_subs_ += thumb_->DragStartedEvent().Subscribe(
        std::bind_front(&TrackBar::OnThumbDragStarted, this));

    thumb_subs_ += thumb_->DraggingEvent().Subscribe(
        std::bind_front(&TrackBar::OnThumbDragging, this));

    thumb_subs_ += thumb_->DragEndedEvent().Subscribe(
        std::bind_front(&TrackBar::OnThumbDragEnded, this));
}


void TrackBar::OnThumbDragStarted(const TrackBarThumbDragStartedInfo& event_info) {

    SetIsFocused(true);

    drag_start_value_ = value_;

    auto mouse_position = GetMousePosition();
    drag_start_position_ = is_horizontal_ ? mouse_position.x : mouse_position.y;
}


void TrackBar::OnThumbDragging(const TrackBarThumbDraggingInfo& event_info) {

    auto content_size = ContentSize();
    float track_length = is_horizontal_ ? content_size.width : content_size.height;
    track_length -= thumb_thickness_;
    if (track_length <= 0) {
        return;
    }

    int value_range = max_value_ - min_value_;
    float value_per_point = static_cast<float>(double(value_range) / track_length);

    auto mouse_position = GetMousePosition();
    auto delta = is_horizontal_ ? mouse_position.x : mouse_position.y;
    delta -= drag_start_position_;

    SetValue(static_cast<int>(drag_start_value_ + delta * value_per_point));
}


void TrackBar::OnThumbDragEnded(const TrackBarThumbDragEndedInfo& event_info) {

}


int TrackBar::Value() const noexcept {
    return value_;
}

void TrackBar::SetValue(int value) {

    auto value_to_set = std::min(value, max_value_);
    value_to_set = std::max(value_to_set, min_value_);
    
    if (value_ == value_to_set) {
        return;
    }

    value_ = value_to_set;
    NeedRelayout();

    value_changed_event_.Raise(TrackBarValueChangedInfo{
        As<TrackBar>(shared_from_this()) 
    });
}


Observable<TrackBarValueChangedInfo> TrackBar::ValueChangedEvent() const {
    return value_changed_event_.GetObservable();
}


int TrackBar::MinValue() const noexcept {
    return min_value_;
}

void TrackBar::SetMinValue(int value) {

    if (min_value_ == value) {
        return;
    }

    min_value_ = value;

    if (max_value_ < min_value_) {
        max_value_ = min_value_;
    }

    if (value_ < min_value_) {
        value_ = min_value_;
    }

    NeedRelayout();
}


int TrackBar::MaxValue() const noexcept {
    return max_value_;
}

void TrackBar::SetMaxValue(int value) {

    if (max_value_ == value) {
        return;
    }

    max_value_ = value;

    if (min_value_ > max_value_) {
        min_value_ = max_value_;
    }

    if (value_ > max_value_) {
        value_ = max_value_;
    }

    NeedRelayout();
}


int TrackBar::LargeChange() const noexcept {
    return large_change_;
}

void TrackBar::SetLargeChange(int change) noexcept {
    large_change_ = change;
}


bool TrackBar::IsHorizontal() const noexcept {
    return is_horizontal_;
}

void TrackBar::SetIsHorizontal(bool is_horizontal) {

    if (is_horizontal_ == is_horizontal) {
        return;
    }

    is_horizontal_ = is_horizontal;
    NeedRelayout();
}


float TrackBar::TrackThickness() const noexcept {
    return track_thickness_;
}

void TrackBar::SetTrackThickness(float thickness) {

    if (track_thickness_ == thickness) {
        return;
    }

    track_thickness_ = thickness;
    NeedRepaint();
}


const Color& TrackBar::TrackColor() const noexcept {
    return track_color_field_.Color();
}

void TrackBar::SetTrackColor(const Color& color) {
    track_color_field_.SetColor(color, *this);
}


const ColorPicker& TrackBar::TrackColorPicker() const noexcept {
    return track_color_field_.ColorPicker();
}

void TrackBar::SetTrackColorPicker(ColorPicker picker) {
    track_color_field_.SetColorPicker(std::move(picker), *this);
}


float TrackBar::ThumbThickness() const noexcept {
    return thumb_thickness_;
}

void TrackBar::SetThumbThickness(float thickness) {

    if (thumb_thickness_ == thickness) {
        return;
    }

    thumb_thickness_ = thickness;
    NeedRelayout();
}


TrackBarValueChangedInfo::TrackBarValueChangedInfo(std::shared_ptr<TrackBar> track_bar) : 
    EventInfo(std::move(track_bar)) {

}

}