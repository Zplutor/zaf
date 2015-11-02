#include <zaf/scroll_bar.h>
#include <zaf/timer.h>
#include <zaf/window.h>
#include <zaf/internal/theme/painter.h>
#include <zaf/internal/theme/theme.h>
#include <zaf/internal/log.h>

namespace zaf {

static int kTimerInitialInterval = 300;
static int kTimerContinuousInterval = 50;

ScrollBar::ScrollBar() : 
	incremental_arrow_(std::make_shared<Arrow>()),
	decremental_arrow_(std::make_shared<Arrow>()),
	thumb_(std::make_shared<Thumb>()),
	is_horizontal_(false),
	arrow_length_(0),
	min_value_(0), 
	max_value_(0), 
	value_(0),
	begin_drag_value_(0),
	timer_event_(TimerEvent::Increment),
	scroll_event_(),
	OnScroll(scroll_event_) {

}


void ScrollBar::Initialize() {

	Control::Initialize();

	InitializeArrow(incremental_arrow_);
	InitializeArrow(decremental_arrow_);
	InitializeThumb(thumb_);
	ApplyOrientationToChildren();
}


void ScrollBar::InitializeArrow(const std::shared_ptr<Arrow>& arrow) {

	arrow->OnBeginPress.AddListenerWithTag(
		std::bind(&ScrollBar::ArrowBeginPress, this, std::placeholders::_1), 
		this
	);

	arrow->OnEndPress.AddListenerWithTag(
		std::bind(&ScrollBar::ArrowEndPress, this, std::placeholders::_1),
		this
	);

	AddChild(arrow);
}


void ScrollBar::UninitializeArrow(const std::shared_ptr<Arrow>& arrow) {

	arrow->OnBeginPress.RemoveListenersWithTag(this);
	arrow->OnEndPress.RemoveListenersWithTag(this);
	RemoveChild(arrow);
}


void ScrollBar::InitializeThumb(const std::shared_ptr<Thumb>& thumb) {

	thumb->OnBeginDrag.AddListenerWithTag(
		std::bind(&ScrollBar::ThumbBeginDrag, this, std::placeholders::_1),
		this
	);

	thumb->OnDrag.AddListenerWithTag(
		std::bind(&ScrollBar::ThumbDrag, this, std::placeholders::_1),
		this
	);

	thumb->OnEndDrag.AddListenerWithTag(
		std::bind(&ScrollBar::ThumbEndDrag, this, std::placeholders::_1),
		this
	);

	AddChild(thumb);
}


void ScrollBar::UninitializeThumb(const std::shared_ptr<Thumb>& thumb) {

	thumb->OnBeginDrag.RemoveListenersWithTag(this);
	thumb->OnDrag.RemoveListenersWithTag(this);
	thumb->OnEndDrag.RemoveListenersWithTag(this);
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

	if (HasChild(previous_arrow)) {
		UninitializeArrow(previous_arrow);
		InitializeArrow(incremental_arrow_);
		ApplyOrientationToChildren();
	}
}


void ScrollBar::SetDecrementalArrow(const std::shared_ptr<ScrollBar::Arrow>& decremental_arrow) {

	auto previous_arrow = decremental_arrow_;
	decremental_arrow_ = decremental_arrow;

	if (HasChild(previous_arrow)) {
		UninitializeArrow(previous_arrow);
		InitializeArrow(decremental_arrow_);
		ApplyOrientationToChildren();
	}
}


void ScrollBar::SetThumb(const std::shared_ptr<ScrollBar::Thumb>& thumb) {

	auto previous_thumb = thumb_;
	thumb_ = thumb;

	if (HasChild(previous_thumb)) {
		UninitializeThumb(previous_thumb);
		InitializeThumb(thumb_);
		ApplyOrientationToChildren();
	}
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
		scroll_event_.Trigger(std::dynamic_pointer_cast<ScrollBar>(shared_from_this()));
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


void ScrollBar::Paint(Canvas& canvas, const Rect& dirty_rect) {
	internal::theme::GetTheme()->GetScrollBarPainter()->Paint(canvas, dirty_rect, *this);
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

	Size arrow_size(bar_thickness, arrow_length_);
	Rect decremental_arrow_rect(Rect(Point(), arrow_size));
	Rect incremental_arrow_rect(Rect(Point(0, bar_length - arrow_length_), arrow_size));

	float thumb_position = 0;
	float thumb_length = 0;
	CalculateThumbPositionAndLength(bar_length - arrow_length_ * 2, thumb_position, thumb_length);

	Rect thumb_rect(0, thumb_position + arrow_length_, bar_thickness, thumb_length);

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


void ScrollBar::MouseDown(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) {

	if (button == MouseButton::Left) {
		CaptureMouse();
		BeginTimer(TimerEvent::PageRoll);
	}
}


void ScrollBar::MouseUp(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) {

	if (button == MouseButton::Left) {
		ReleaseMouse();
		timer_.reset();
	}
}


void ScrollBar::BeginTimer(TimerEvent timer_event) {

	timer_event_ = timer_event;
	ApplyTimerEvent();

	timer_ = std::make_unique<Timer>(
		kTimerInitialInterval,
		std::bind(&ScrollBar::TimerTrigger, this, std::placeholders::_1)
	);
	timer_->Start();
}


void ScrollBar::TimerTrigger(Timer&) {

	ApplyTimerEvent();

	if (timer_->GetInterval() == kTimerInitialInterval) {
		timer_ = std::make_unique<Timer>(
			kTimerContinuousInterval,
			std::bind(&ScrollBar::TimerTrigger, this, std::placeholders::_1)
		);
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


void ScrollBar::MouseWheel(const Point& position, bool is_horizontal, int distance, WPARAM wParam, LPARAM lParam) {

	if (is_horizontal != IsHorizontal()) {
		return;
	}
	
	int major_distance = distance / WHEEL_DELTA;
	
	int minor_distance = 0;
	if (distance % WHEEL_DELTA != 0) {
		minor_distance = distance > 0 ? 1 : -1;
	}

	int direction = is_horizontal ? 1 : -1;

	SetValue(value_ + (major_distance + minor_distance) * direction);
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
		SetValue(begin_drag_value_ + mouse_offset * value_per_point);
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
	thumb_scroll_length -= arrow_length_ * 2;

	int value_range = max_value_ - min_value_;
	return value_range / thumb_scroll_length;
}


ScrollBar::Arrow::Arrow() : 
	direction_(Direction::Up),
	begin_press_event_(),
	OnBeginPress(begin_press_event_),
	end_press_event_(),
	OnEndPress(end_press_event_) {

	SetCanFocused(false);
}


void ScrollBar::Arrow::Paint(Canvas& canvas, const Rect& dirty_rect) {
	internal::theme::GetTheme()->GetScrollBarArrowPainter()->Paint(canvas, dirty_rect, *this);
}


void ScrollBar::Arrow::MouseDown(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) {

	Button::MouseDown(position, button, wParam, lParam);

	if (button == MouseButton::Left) {
		begin_press_event_.Trigger(std::dynamic_pointer_cast<Arrow>(this->shared_from_this()));
	}
}


void ScrollBar::Arrow::MouseUp(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) {

	Button::MouseUp(position, button, wParam, lParam);

	if (button == MouseButton::Left) {
		end_press_event_.Trigger(std::dynamic_pointer_cast<Arrow>(this->shared_from_this()));
	}
}


ScrollBar::Thumb::Thumb() : 
	is_dragging_(false),
	begin_drag_event_(),
	OnBeginDrag(begin_drag_event_),
	drag_event_(),
	OnDrag(drag_event_),
	end_drag_event_(),
	OnEndDrag(end_drag_event_) {

	SetCanFocused(false);
}


void ScrollBar::Thumb::Paint(Canvas& canvas, const Rect& dirty_rect) {
	internal::theme::GetTheme()->GetScrollBarThumbPainter()->Paint(canvas, dirty_rect, *this);
}


void ScrollBar::Thumb::MouseDown(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) {

	Button::MouseDown(position, button, wParam, lParam);

	if (button == MouseButton::Left) {
		is_dragging_ = true;
		begin_drag_event_.Trigger(std::dynamic_pointer_cast<Thumb>(shared_from_this()));
	}
}


void ScrollBar::Thumb::MouseUp(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) {

	Button::MouseUp(position, button, wParam, lParam);

	if (button == MouseButton::Left) {
		is_dragging_ = false;
		end_drag_event_.Trigger(std::dynamic_pointer_cast<Thumb>(shared_from_this()));
	}
}


void ScrollBar::Thumb::MouseMove(const Point& position, WPARAM wParam, LPARAM lParam) {

	Button::MouseMove(position, wParam, lParam);

	if (is_dragging_) {
		drag_event_.Trigger(std::dynamic_pointer_cast<Thumb>(shared_from_this()));
	}
}

}