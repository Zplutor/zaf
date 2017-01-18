#include <zaf/control/scroll_bar.h>
#include <zaf/base/event_utility.h>
#include <zaf/base/timer.h>
#include <zaf/creation.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/geometry/geometry_sink.h>
#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/geometry/transformed_geometry.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/internal/theme.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {

static const wchar_t* const kArrowLengthPropertyName = L"ArrowLength";
static const wchar_t* const kArrowColorPickerPropertyName = L"ArrowColorPicker";
static const wchar_t* const kScrollEventPropertyName = L"ScrollEvent";
static const wchar_t* const kThumbColorPickerProeprtyName = L"ThumbColorPicker";

static const int kTimerInitialInterval = 300;
static const int kTimerContinuousInterval = 50;

ScrollBar::ScrollBar() : 
	incremental_arrow_(Create<Arrow>()),
	decremental_arrow_(Create<Arrow>()),
	thumb_(Create<Thumb>()),
	is_horizontal_(false),
	min_value_(0), 
	max_value_(0), 
	value_(0),
	begin_drag_value_(0),
	timer_event_(TimerEvent::Increment) {

}


void ScrollBar::Initialize() {

	__super::Initialize();

	InitializeArrow(incremental_arrow_);
	InitializeArrow(decremental_arrow_);
	InitializeThumb(thumb_);
	ApplyOrientationToChildren();
}


void ScrollBar::InitializeArrow(const std::shared_ptr<Arrow>& arrow) {

    arrow->GetBeginPressEvent().AddListenerWithTag(
        reinterpret_cast<std::uintptr_t>(this),
		std::bind(&ScrollBar::ArrowBeginPress, this, std::placeholders::_1) 
	);

    arrow->GetEndPressEvent().AddListenerWithTag(
        reinterpret_cast<std::uintptr_t>(this),
		std::bind(&ScrollBar::ArrowEndPress, this, std::placeholders::_1)
	);

	AddChild(arrow);
}


void ScrollBar::UninitializeArrow(const std::shared_ptr<Arrow>& arrow) {

    arrow->GetBeginPressEvent().RemoveListenersWithTag(reinterpret_cast<std::uintptr_t>(this));
    arrow->GetEndPressEvent().RemoveListenersWithTag(reinterpret_cast<std::uintptr_t>(this));
	RemoveChild(arrow);
}


void ScrollBar::InitializeThumb(const std::shared_ptr<Thumb>& thumb) {

	thumb->GetBeginDragEvent().AddListenerWithTag(
        reinterpret_cast<std::uintptr_t>(this),
		std::bind(&ScrollBar::ThumbBeginDrag, this, std::placeholders::_1)
	);

    thumb->GetDragEvent().AddListenerWithTag(
        reinterpret_cast<std::uintptr_t>(this),
		std::bind(&ScrollBar::ThumbDrag, this, std::placeholders::_1)
	);

    thumb->GetEndDragEvent().AddListenerWithTag(
        reinterpret_cast<std::uintptr_t>(this),
		std::bind(&ScrollBar::ThumbEndDrag, this, std::placeholders::_1)
	);

	AddChild(thumb);
}


void ScrollBar::UninitializeThumb(const std::shared_ptr<Thumb>& thumb) {

    auto tag = reinterpret_cast<std::uintptr_t>(this);
    thumb->GetBeginDragEvent().RemoveListenersWithTag(tag);
    thumb->GetDragEvent().RemoveListenersWithTag(tag);
    thumb->GetEndDragEvent().RemoveListenersWithTag(tag);
	RemoveChild(thumb);
}


void ScrollBar::ApplyOrientationToChildren() {

	incremental_arrow_->SetDirection(
		is_horizontal_ ? Arrow::Direction::Right : Arrow::Direction::Down
	);
	decremental_arrow_->SetDirection(
		is_horizontal_ ? Arrow::Direction::Left : Arrow::Direction::Up
	);
	thumb_->SetIsHorizontal(is_horizontal_);
}


void ScrollBar::SetIncrementalArrow(const std::shared_ptr<ScrollBar::Arrow>& incremental_arrow) {

	auto previous_arrow = incremental_arrow_;
	incremental_arrow_ = incremental_arrow;

	if (IsParentOf(previous_arrow)) {
		UninitializeArrow(previous_arrow);
		InitializeArrow(incremental_arrow_);
		ApplyOrientationToChildren();
	}
}


void ScrollBar::SetDecrementalArrow(const std::shared_ptr<ScrollBar::Arrow>& decremental_arrow) {

	auto previous_arrow = decremental_arrow_;
	decremental_arrow_ = decremental_arrow;

	if (IsParentOf(previous_arrow)) {
		UninitializeArrow(previous_arrow);
		InitializeArrow(decremental_arrow_);
		ApplyOrientationToChildren();
	}
}


void ScrollBar::SetThumb(const std::shared_ptr<ScrollBar::Thumb>& thumb) {

	auto previous_thumb = thumb_;
	thumb_ = thumb;

	if (IsParentOf(previous_thumb)) {
		UninitializeThumb(previous_thumb);
		InitializeThumb(thumb_);
		ApplyOrientationToChildren();
	}
}


float ScrollBar::GetArrowLength() const {

    auto length = GetPropertyMap().TryGetProperty<float>(kArrowLengthPropertyName);
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

    GetPropertyMap().SetProperty(kArrowLengthPropertyName, length);
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

        auto event = TryGetEventFromPropertyMap<ScrollEvent>(GetPropertyMap(), kScrollEventPropertyName);
		if (event != nullptr) {
			event->Trigger(std::dynamic_pointer_cast<ScrollBar>(shared_from_this()));
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


void ScrollBar::Wheel(int distance) {

    int major_distance = distance / WHEEL_DELTA;

    int minor_distance = 0;
    if (distance % WHEEL_DELTA != 0) {
        minor_distance = distance > 0 ? 1 : -1;
    }

    int direction = is_horizontal_ ? 1 : -1;

    SetValue(value_ + (major_distance + minor_distance) * direction);
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


ScrollBar::ScrollEvent::Proxy ScrollBar::GetScrollEvent() {
    return GetEventProxyFromPropertyMap<ScrollEvent>(GetPropertyMap(), kScrollEventPropertyName);
}


void ScrollBar::MouseDown(const Point& position, const MouseMessage& message) {

	if (message.GetMouseButton() == MouseButton::Left) {
		CaptureMouse();
	}
}


void ScrollBar::MouseUp(const Point& position, const MouseMessage& message) {

	if (message.GetMouseButton() == MouseButton::Left) {
		ReleaseMouse();
	}
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
    timer_->GetTriggerEvent().AddListener(std::bind(&ScrollBar::TimerTrigger, this, std::placeholders::_1));
	timer_->Start();
}


void ScrollBar::TimerTrigger(Timer&) {

	ApplyTimerEvent();

	if (timer_->GetInterval() == std::chrono::milliseconds(kTimerInitialInterval)) {

        timer_ = std::make_shared<Timer>(Timer::Mode::DeferredRepeated);
        timer_->SetInterval(std::chrono::milliseconds(kTimerContinuousInterval));
        timer_->GetTriggerEvent().AddListener(std::bind(&ScrollBar::TimerTrigger, this, std::placeholders::_1));
		timer_->Start();
	}
}


void ScrollBar::ApplyTimerEvent() {

	if (timer_event_ == TimerEvent::Increment) {

		if (incremental_arrow_->IsPressed()) {
			SetValue(value_ + 1);
		}
	}
	else if (timer_event_ == TimerEvent::Decrement) {

		if (decremental_arrow_->IsPressed()) {
			SetValue(value_ - 1);
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
			SetValue(value_ - 10);
		}
		else {
			SetValue(value_ + 10);
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


void ScrollBar::MouseWheel(const Point& position, const MouseWheelMessage& message) {

	if (message.IsHorizontalWheeling() != IsHorizontal()) {
		return;
	}
	
    Wheel(message.GetWheelingDistance());
}


void ScrollBar::ArrowBeginPress(const std::shared_ptr<Arrow>& arrow) {

	if (arrow == incremental_arrow_) {
		BeginTimer(TimerEvent::Increment);
	}
	else if (arrow == decremental_arrow_) {
		BeginTimer(TimerEvent::Decrement);
	}
}


void ScrollBar::ArrowEndPress(const std::shared_ptr<Arrow>& arrow) {
	timer_.reset();
}


void ScrollBar::ThumbBeginDrag(const std::shared_ptr<Thumb>& thumb) {

	begin_drag_value_ = value_;
	begin_drag_mouse_position_ = GetMousePosition();
}


void ScrollBar::ThumbDrag(const std::shared_ptr<Thumb>& thumb) {

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


void ScrollBar::ThumbEndDrag(const std::shared_ptr<Thumb>& thumb) {

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


ScrollBar::Arrow::Arrow() : direction_(Direction::Up) {

}


void ScrollBar::Arrow::Initialize() {

	__super::Initialize();

	SetCanFocused(false);
}


void ScrollBar::Arrow::Paint(Canvas& canvas, const Rect& dirty_rect) {
	
	__super::Paint(canvas, dirty_rect);

	Canvas::StateGuard state_guard(canvas);

    //Firstly, create a geometry that describes an triangle arrow which direction is up,
    //and then rotate the geometry according to the actual direction.

    Direction direction = GetDirection();

    float bottom_edge_length =
        (direction == Direction::Up || direction == Direction::Down) ? GetWidth() : GetHeight();
    bottom_edge_length /= 2;
    float height = bottom_edge_length / 2;
    float half_height =  height / 2;

    Point center_point(GetWidth() / 2, GetHeight() / 2);

    Point top_point(center_point.x, center_point.y - half_height);
    Point left_point(center_point.x - height, center_point.y + half_height);
    Point right_point(center_point.x + height, center_point.y + half_height);

    auto triangle_geometry = GetResourceFactory()->CreatePathGeometry();
    if (triangle_geometry == nullptr) {
		return;
	}

    auto triangle_geometry_sink = triangle_geometry.Open();
    if (triangle_geometry_sink == nullptr) {
		return;
	}

    triangle_geometry_sink.BeginFigure(top_point, GeometrySink::BeginFigureOption::Fill);
    triangle_geometry_sink.AddLine(left_point);
    triangle_geometry_sink.AddLine(right_point);
    triangle_geometry_sink.EndFigure(GeometrySink::EndFigureOption::Close);
    triangle_geometry_sink.Close();

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

    auto rotation_geometry = GetResourceFactory()->CreateTransformedGeometry(
        triangle_geometry, 
        TransformMatrix::Rotation(rotate_angle, center_point));

	canvas.SetBrushWithColor(GetArrowColor());
	canvas.DrawGeometry(rotation_geometry);
}


const ColorPicker ScrollBar::Arrow::GetArrowColorPicker() const {

    auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(kArrowColorPickerPropertyName);
    if ( (color_picker != nullptr) && (*color_picker != nullptr) ) {
        return *color_picker;
    }
    else {

        return [](const Control& control) {

            const auto& arrow = dynamic_cast<const Arrow&>(control);

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


void ScrollBar::Arrow::SetArrowColorPicker(const ColorPicker& color_picker) {

    GetPropertyMap().SetProperty(kArrowColorPickerPropertyName, color_picker);
    NeedRepaint();
}


void ScrollBar::Arrow::MouseCapture() {

	ClickableControl::MouseCapture();

	begin_press_event_.Trigger(std::dynamic_pointer_cast<Arrow>(this->shared_from_this()));
}


void ScrollBar::Arrow::MouseRelease() {

	ClickableControl::MouseRelease();

	end_press_event_.Trigger(std::dynamic_pointer_cast<Arrow>(this->shared_from_this()));
}


ScrollBar::Thumb::Thumb() : is_dragging_(false) {

}


void ScrollBar::Thumb::Initialize() {

	__super::Initialize();

	SetCanFocused(false);
}


void ScrollBar::Thumb::Paint(Canvas& canvas, const Rect& dirty_rect) {

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


const ColorPicker ScrollBar::Thumb::GetThumbColorPicker() const {

    auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(kThumbColorPickerProeprtyName);
    if (color_picker != nullptr && *color_picker != nullptr) {
        return *color_picker;
    }
    else {

        return [](const Control& control) {

            const auto& thumb = dynamic_cast<const Thumb&>(control);

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


void ScrollBar::Thumb::SetThumbColorPicker(const ColorPicker& color_picker) {

    GetPropertyMap().SetProperty(kThumbColorPickerProeprtyName, color_picker);
    NeedRepaint();
}


void ScrollBar::Thumb::MouseCapture() {

	ClickableControl::MouseCapture();

	is_dragging_ = true;
	begin_drag_event_.Trigger(std::dynamic_pointer_cast<Thumb>(shared_from_this()));
}


void ScrollBar::Thumb::MouseRelease() {

	ClickableControl::MouseRelease();

	is_dragging_ = false;
	end_drag_event_.Trigger(std::dynamic_pointer_cast<Thumb>(shared_from_this()));
}


void ScrollBar::Thumb::MouseMove(const Point& position, const MouseMessage& message) {

	ClickableControl::MouseMove(position, message);

	if (is_dragging_) {
		drag_event_.Trigger(std::dynamic_pointer_cast<Thumb>(shared_from_this()));
	}
}

}