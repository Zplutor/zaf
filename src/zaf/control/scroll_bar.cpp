#include <zaf/control/scroll_bar.h>
#include <zaf/base/event_utility.h>
#include <zaf/base/timer.h>
#include <zaf/control/internal/triangle_geometry.h>
#include <zaf/creation.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/geometry/geometry_sink.h>
#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/geometry/transformed_geometry.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/internal/theme.h>
#include <zaf/parsing/parsers/scroll_bar_arrow_parser.h>
#include <zaf/parsing/parsers/scroll_bar_parser.h>
#include <zaf/parsing/parsers/scroll_bar_thumb_parser.h>
#include <zaf/reflection/reflection_type_definition.h>
#include <zaf/serialization/properties.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {
namespace {

constexpr wchar_t* const kScrollEventPropertyName = L"ScrollEvent";

constexpr int kTimerInitialInterval = 300;
constexpr int kTimerContinuousInterval = 50;

}

ZAF_DEFINE_REFLECTION_TYPE(ScrollBar)
	ZAF_DEFINE_PARSER(ScrollBarParser)
ZAF_DEFINE_END

ZAF_DEFINE_REFLECTION_TYPE(ScrollBarArrow)
	ZAF_DEFINE_PARSER(ScrollBarArrowParser)
ZAF_DEFINE_END

ZAF_DEFINE_REFLECTION_TYPE(ScrollBarThumb)
	ZAF_DEFINE_PARSER(ScrollBarThumbParser)
ZAF_DEFINE_END


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
		is_horizontal_ ? ScrollBarArrow::Direction::Right : ScrollBarArrow::Direction::Down
	);
	decremental_arrow_->SetDirection(
		is_horizontal_ ? ScrollBarArrow::Direction::Left : ScrollBarArrow::Direction::Up
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


float ScrollBar::GetArrowLength() const {

    auto length = GetPropertyMap().TryGetProperty<float>(property::ArrowLength);
    if (length != nullptr) {
        return *length;
    }
        
    if (IsHorizontal()) {
        return GetHeight();
    }
    else {
        return GetWidth();
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
			ScrollBarScrollInfo event_info;
			event_info.scroll_bar = std::dynamic_pointer_cast<ScrollBar>(shared_from_this());
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

	SetValue(value_);
	NeedRelayout();
}


int ScrollBar::GetSmallChangeValue() const {

    auto value = GetPropertyMap().TryGetProperty<int>(property::SmallChangeValue);
    if (value != nullptr) {
        return *value;
    }
    else {
        return 1;
    }
}

void ScrollBar::SetSmallChangeValue(int value) {
    GetPropertyMap().SetProperty(property::SmallChangeValue, value);
}


int ScrollBar::GetLargeChangeValue() const {

    auto value = GetPropertyMap().TryGetProperty<int>(property::LargeChangeValue);
    if (value != nullptr) {
        return *value;
    }
    else {
        return 10;
    }
}

void ScrollBar::SetLargeChangeValue(int value) {
    GetPropertyMap().SetProperty(property::LargeChangeValue, value);
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
        change_value = GetLargeChangeValue();
        multiple = 1;
    }
    else {
        change_value = GetSmallChangeValue();
    }
    
    double expected_change_value = change_count * change_value * multiple;
    expected_change_value += remaining_wheel_change_value_;

    int actual_change_value = static_cast<int>(expected_change_value);
    remaining_wheel_change_value_ = expected_change_value - actual_change_value;
    
    int direction = IsHorizontal() ? 1 : -1;
    SetValue(value_ + actual_change_value * direction);
}


void ScrollBar::Layout(const Rect& previous_rect) {

	const Rect& rect = GetRect();

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

    float arrow_length = GetArrowLength();
	Size arrow_size(bar_thickness, arrow_length);
	Rect decremental_arrow_rect(Rect(Point(), arrow_size));
    Rect incremental_arrow_rect(Rect(Point(0, bar_length - arrow_length), arrow_size));

	float thumb_position = 0;
	float thumb_length = 0;
    CalculateThumbPositionAndLength(bar_length - arrow_length * 2, thumb_position, thumb_length);

    Rect thumb_rect(0, thumb_position + arrow_length, bar_thickness, thumb_length);

	if (is_horizontal_) {
		ChangeVerticalRectToHorizontalRect(incremental_arrow_rect);
		ChangeVerticalRectToHorizontalRect(decremental_arrow_rect);
		ChangeVerticalRectToHorizontalRect(thumb_rect);
	}

	incremental_arrow_->SetRect(incremental_arrow_rect);
	decremental_arrow_->SetRect(decremental_arrow_rect);
	thumb_->SetRect(thumb_rect);
}


void ScrollBar::CalculateThumbPositionAndLength(float slot_length, 
												float& thumb_position, 
												float& thumb_length) {

	const float min_length = 10;

	//Don't display the thumb if there is no enough space.
	if (slot_length < min_length) {
		thumb_position = 0;
		thumb_length = 0;
		return;
	}

	int position = value_ - min_value_;
	int range = max_value_ - min_value_;

	if (range < slot_length - min_length) {

		thumb_position = static_cast<float>(position);
		thumb_length = slot_length - range;
	}
	else {

		thumb_length = min_length;

		float position_percent = static_cast<float>(position) / static_cast<float>(range);
		thumb_position = (slot_length - thumb_length) * position_percent;
	}
}


void ScrollBar::ChangeVerticalRectToHorizontalRect(Rect& rect) {

	std::swap(rect.position.x, rect.position.y);
	std::swap(rect.size.width, rect.size.height);
}


Observable<ScrollBarScrollInfo> ScrollBar::ScrollEvent() {
    return GetEventObservable<ScrollBarScrollInfo>(GetPropertyMap(), kScrollEventPropertyName);
}


bool ScrollBar::MouseDown(const Point& position, const MouseMessage& message) {

	if (message.GetMouseButton() == MouseButton::Left) {
		CaptureMouse();
	}
    return true;
}


bool ScrollBar::MouseUp(const Point& position, const MouseMessage& message) {

	if (message.GetMouseButton() == MouseButton::Left) {
		ReleaseMouse();
	}
    return true;
}


void ScrollBar::MouseCapture() {
	BeginTimer(TimerEvent::PageRoll);
}


void ScrollBar::MouseRelease() {
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
			SetValue(value_ + GetSmallChangeValue());
		}
	}
	else if (timer_event_ == TimerEvent::Decrement) {

		if (decremental_arrow_->IsPressed()) {
            SetValue(value_ - GetSmallChangeValue());
		}
	}
	else if (timer_event_ == TimerEvent::PageRoll) {

		Point mouse_position = GetMousePosition();

		Rect thumb_slot_rect = GetThumbSlotRect();
		if (! thumb_slot_rect.Contain(mouse_position)) {
			return;
		}

		Rect thumb_rect = thumb_->GetRect();
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
			SetValue(value_ - GetLargeChangeValue());
		}
		else {
			SetValue(value_ + GetLargeChangeValue());
		}
	}
}


Rect ScrollBar::GetThumbSlotRect() const {

	Rect thumb_slot_rect(Point(), GetSize());

	if (is_horizontal_) {

		float arrow_thickness = incremental_arrow_->GetSize().width;
		thumb_slot_rect.position.x += arrow_thickness;
		thumb_slot_rect.size.width -= arrow_thickness;
	}
	else {

		float arrow_thickness = incremental_arrow_->GetSize().height;
		thumb_slot_rect.position.y += arrow_thickness;
		thumb_slot_rect.size.height -= arrow_thickness;
	}

	return thumb_slot_rect;
}


bool ScrollBar::MouseWheel(const Point& position, const MouseWheelMessage& message) {

	if (message.IsHorizontalWheeling() == IsHorizontal()) {
        Wheel(message.GetWheelingDistance());
	}
    return true;
}


void ScrollBar::ArrowBeginPress(const ScrollBarArrowBeginPressInfo& event_info) {

	if (event_info.scroll_bar_arrow == incremental_arrow_) {
		BeginTimer(TimerEvent::Increment);
	}
	else if (event_info.scroll_bar_arrow == decremental_arrow_) {
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
		int value_per_point = GetValuesPerThumbSlotPoint();
		SetValue(static_cast<int>(begin_drag_value_ + mouse_offset * value_per_point));
	}
}


void ScrollBar::ThumbEndDrag(const ScrollBarThumbEndDragInfo& event_info) {

}


int ScrollBar::GetValuesPerThumbSlotPoint() {

	const Size& bar_size = GetSize();
	const Size& thumb_size = thumb_->GetSize();

	float thumb_scroll_length = 0;
	if (is_horizontal_) {
		thumb_scroll_length = bar_size.width - thumb_size.width;
	}
	else {
		thumb_scroll_length = bar_size.height - thumb_size.height;
	}
    thumb_scroll_length -= GetArrowLength() * 2;

	int value_range = max_value_ - min_value_;
	return static_cast<int>(value_range / thumb_scroll_length);
}


ScrollBarArrow::ScrollBarArrow() : direction_(Direction::Up) {

}


void ScrollBarArrow::Initialize() {

	__super::Initialize();

	SetCanFocused(false);
}


void ScrollBarArrow::Paint(Canvas& canvas, const Rect& dirty_rect) {
	
	__super::Paint(canvas, dirty_rect);

    Direction direction = GetDirection();

	float bottom_edge_length =
		(direction == Direction::Up || direction == Direction::Down) ? GetWidth() : GetHeight();

	bottom_edge_length /= 2;

	float rotate_angle = 0;
	switch (direction) {
	case Direction::Left:
		rotate_angle = 270;
		break;
	case Direction::Right:
		rotate_angle = 90;
		break;
	case Direction::Down:
		rotate_angle = 180;
		break;
	default:
		break;
	}

	auto triangle_geometry = internal::CreateTriangleGeometry(
		GetWidth(), 
		GetHeight(), 
		bottom_edge_length, 
		rotate_angle);

	Canvas::StateGuard state_guard(canvas);
	canvas.SetBrushWithColor(GetArrowColor());
	canvas.DrawGeometry(triangle_geometry);
}


ColorPicker ScrollBarArrow::GetArrowColorPicker() const {

    auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(property::ArrowColorPicker);
    if ( (color_picker != nullptr) && (*color_picker != nullptr) ) {
        return *color_picker;
    }
    else {

        return [](const Control& control) {

            const auto& arrow = dynamic_cast<const ScrollBarArrow&>(control);

            if (! arrow.IsEnabled()) {
                return Color::FromRGB(0xc0c0c0);
            }

            if (arrow.IsPressed()) {
                return Color::FromRGB(0x306DD9);
            }

            if (arrow.IsHovered()) {
                return Color::FromRGB(0x5080ef);
            }

            return Color::FromRGB(0x808080);
        };
    }
}


void ScrollBarArrow::SetArrowColorPicker(const ColorPicker& color_picker) {

    GetPropertyMap().SetProperty(property::ArrowColorPicker, color_picker);
    NeedRepaint();
}


void ScrollBarArrow::MouseCapture() {

	ClickableControl::MouseCapture();

	ScrollBarArrowBeginPressInfo event_info;
	event_info.scroll_bar_arrow = std::dynamic_pointer_cast<ScrollBarArrow>(shared_from_this());
	begin_press_event_.GetObserver().OnNext(event_info);
}


void ScrollBarArrow::MouseRelease() {

	ClickableControl::MouseRelease();

	ScrollBarArrowEndPressInfo event_info;
	event_info.scroll_bar_arrow = std::dynamic_pointer_cast<ScrollBarArrow>(shared_from_this());
	end_press_event_.GetObserver().OnNext(event_info);
}


ScrollBarThumb::ScrollBarThumb() : 
	is_dragging_(false) {

}


void ScrollBarThumb::Initialize() {

	__super::Initialize();

	SetCanFocused(false);
}


void ScrollBarThumb::Paint(Canvas& canvas, const Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    if (! IsEnabled()) {
        return;
    }

    Canvas::StateGuard state_guard(canvas);

    auto thumb_rect = GetContentRect();
    const float spacing = 2;
    if (IsHorizontal()) {
        thumb_rect.Inflate(0, -spacing);
    }
    else {
        thumb_rect.Inflate(-spacing, 0);
    }

    canvas.SetBrushWithColor(GetThumbColor());
    canvas.DrawRectangle(thumb_rect);
}


ColorPicker ScrollBarThumb::GetThumbColorPicker() const {

    auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(property::ThumbColorPicker);
    if (color_picker != nullptr && *color_picker != nullptr) {
        return *color_picker;
    }
    else {

        return [](const Control& control) {

            const auto& thumb = dynamic_cast<const ScrollBarThumb&>(control);

            if (thumb.IsPressed()) {
                return Color::FromRGB(0x808080);
            }

            if (thumb.IsHovered()) {
                return Color::FromRGB(0xA9A9A9);
            }

            return Color::FromRGB(0xCECECE);
        };
    }
}


void ScrollBarThumb::SetThumbColorPicker(const ColorPicker& color_picker) {

    GetPropertyMap().SetProperty(property::ThumbColorPicker, color_picker);
    NeedRepaint();
}


void ScrollBarThumb::MouseCapture() {

	ClickableControl::MouseCapture();

	is_dragging_ = true;

	ScrollBarThumbBeginDragInfo event_info;
	event_info.scroll_bar_thumb = std::dynamic_pointer_cast<ScrollBarThumb>(shared_from_this());
	begin_drag_event_.GetObserver().OnNext(event_info);
}


void ScrollBarThumb::MouseRelease() {

	ClickableControl::MouseRelease();

	is_dragging_ = false;

	ScrollBarThumbEndDragInfo event_info;
	event_info.scroll_bar_thumb = std::dynamic_pointer_cast<ScrollBarThumb>(shared_from_this());
	end_drag_event_.GetObserver().OnNext(event_info);
}


bool ScrollBarThumb::MouseMove(const Point& position, const MouseMessage& message) {

	bool result = ClickableControl::MouseMove(position, message);

	if (is_dragging_) {

		ScrollBarThumbDragInfo event_info;
		event_info.scroll_bar_thumb = std::dynamic_pointer_cast<ScrollBarThumb>(shared_from_this());
		drag_event_.GetObserver().OnNext(event_info);
	}

    return result;
}


}