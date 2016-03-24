#include <zaf/control/radio_button.h>
#include <algorithm>
#include <zaf/base/assert.h>
#include <zaf/control/paint_utility.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

static const wchar_t* const kCanAutoSelectPropertyName = L"CanAutoSelect";
static const wchar_t* const kDisabledRadioColorPropertyName = L"DisabledRadioColor";
static const wchar_t* const kFocusedRadioColorPropertyName = L"FocusedRadioColor";
static const wchar_t* const kHoveredRadioColorPropertyName = L"HoveredRadioColor";
static const wchar_t* const kNormalRadioColorPropertyName = L"NormalRadioColor";
static const wchar_t* const kPressedRadioColorPropertyName = L"PressedRadioColor";

RadioButton::RadioButton() : 
	is_selected_(true),
	OnSelectStateChange(select_state_change_event_) {

}


RadioButton::~RadioButton() {

}


void RadioButton::Initialize() {

	__super::Initialize();

	SetColor(PaintComponent::Radio, PaintState::Normal, Color::Black);
	SetColor(PaintComponent::Radio, PaintState::Hovered, Color::FromRGB(0x4169E1));
	SetColor(PaintComponent::Radio, PaintState::Pressed, Color::FromRGB(0x0000CD));
	SetColor(PaintComponent::Radio, PaintState::Disabled, Color::Gray);
}


void RadioButton::Paint(Canvas& canvas, const Rect& dirty_rect) {

	__super::Paint(canvas, dirty_rect);

	auto text_layout = CreateTextLayout(Size());
	if (text_layout == nullptr) {
		return;
	}

	canvas.SetClearEdgeOption(ClearEdgeOption::Clear);

	PaintTextWithIcon(
		canvas,
		GetContentRect(),
		text_layout,
		12, 
		[this](Canvas& canvas, const Rect& text_rect, const std::shared_ptr<TextLayout>& text_layout) {
			canvas.SetBrushWithColor(GetColor(PaintComponent::Foreground, GetPaintState()));
			canvas.DrawText(text_layout, text_rect.position);
		},
		std::bind(&RadioButton::PaintRadio, this, std::placeholders::_1, std::placeholders::_2)
	);
}


void RadioButton::PaintRadio(Canvas& canvas, const Rect& radio_rect) {

	Color box_color = GetColor(PaintComponent::Radio, GetPaintState());
	canvas.SetBrushWithColor(box_color);

	float ellipse_x_radius = radio_rect.size.width / 2;
	float ellipse_y_radius = radio_rect.size.height / 2;

	Point ellipse_position = radio_rect.position;
	ellipse_position.x += ellipse_x_radius;
	ellipse_position.y += ellipse_y_radius;

	Ellipse ellipse(ellipse_position, ellipse_x_radius, ellipse_y_radius);
	canvas.DrawEllipseFrame(ellipse, 1);
	
	if (IsSelected()) {
		ellipse.Inflate(-3);
		canvas.DrawEllipse(ellipse);
	}
}


const Color RadioButton::GetColor(int paint_component, int paint_state) const {

	if (paint_component == PaintComponent::Radio) {

		return GetPropertyMap().GetProperty<Color>(
			GetRadioColorPropertyName(paint_state), 
			[this, paint_state]() {
				
				if (paint_state == PaintState::Normal) {
					return GetColor(PaintComponent::Foreground, PaintState::Normal);
				}
				else if (paint_state == PaintState::Pressed) {
					return GetColor(PaintComponent::Radio, PaintState::Hovered);
				}
				else {
					return GetColor(PaintComponent::Radio, PaintState::Normal);
				}
			}
		);
	}
	else {

		return __super::GetColor(paint_component, paint_state);
	}
}


void RadioButton::SetColor(int paint_component, int paint_state, const Color& new_color) {
	
	if (paint_component == PaintComponent::Radio) {
		GetPropertyMap().SetProperty(GetRadioColorPropertyName(paint_state), new_color);
	}
	else {
		__super::SetColor(paint_component, paint_state, new_color);
	}
}


std::wstring RadioButton::GetRadioColorPropertyName(int paint_state) const {

	switch (paint_state) {

		case PaintState::Normal:
			return kNormalRadioColorPropertyName;

		case PaintState::Hovered:
			return kHoveredRadioColorPropertyName;

		case PaintState::Pressed:
			return kPressedRadioColorPropertyName;

		case PaintState::Focused:
			return kFocusedRadioColorPropertyName;

		case PaintState::Disabled:
			return kDisabledRadioColorPropertyName;

		default:
			ZAF_FAIL();
			return std::wstring();
	}
}


void RadioButton::SetGroup(const std::shared_ptr<Group>& group) {

	if (group_ == group) {
		return;
	}

	auto shared_this = std::dynamic_pointer_cast<RadioButton>(shared_from_this());

	if (group_ != nullptr) {
		group_->Remove(shared_this);
	}

	group_ = group;
	if (group_ != nullptr) {
		group_->Add(shared_this);
	}
}


bool RadioButton::CanAutoSelect() const {
	return GetPropertyMap().GetProperty<bool>(
		kCanAutoSelectPropertyName,
		[]() { return true; }
	);
}


void RadioButton::SetCanAutoSelect(bool can_change) {
	GetPropertyMap().SetProperty(kCanAutoSelectPropertyName, can_change);
}


void RadioButton::SetIsSelected(bool is_selected) {

	if (is_selected == IsSelected()) {
		return;
	}

	is_selected_ = is_selected;
	NeedRepaint();

	auto shared_this = std::dynamic_pointer_cast<RadioButton>(shared_from_this());

	if (is_selected && group_ != nullptr) {
		group_->RadioButtonSelected(shared_this);
	}

	select_state_change_event_.Trigger(shared_this);
}


void RadioButton::MouseClick() {

	if (CanAutoSelect()) {
		SetSelected();
	}
}


const std::vector<std::shared_ptr<RadioButton>> RadioButton::Group::GetRadioButtons() const {

	std::vector<std::shared_ptr<RadioButton>> radio_buttons;

	for (const auto& each_radio_button : radio_buttons_) {

		auto radio_button = each_radio_button.lock();
		if (radio_button != nullptr) {
			radio_buttons.push_back(radio_button);
		}
	}

	return radio_buttons;
}


void RadioButton::Group::Add(const std::shared_ptr<RadioButton>& radio_button) {

	radio_buttons_.push_back(radio_button);

	if (radio_buttons_.size() == 1) {
		radio_button->SetSelected();
	}
	else {
		radio_button->SetUnselected();
	}
}


void RadioButton::Group::Remove(const std::shared_ptr<RadioButton>& radio_button) {

	auto iterator = std::remove_if(
		radio_buttons_.begin(), 
		radio_buttons_.end(), 
		[&radio_button](const std::weak_ptr<RadioButton>& each_radio_button) {
			return each_radio_button.lock() == radio_button;
		}
	);

	radio_buttons_.erase(iterator, radio_buttons_.end());

	if (radio_button->IsSelected() && ! radio_buttons_.empty()) {

		auto first_radio_button = radio_buttons_.front().lock();
		first_radio_button->SetSelected();
	}
}


void RadioButton::Group::RadioButtonSelected(const std::shared_ptr<RadioButton>& selected_radio_button) {

	for (const auto& each_radio_button : radio_buttons_) {

		auto radio_button = each_radio_button.lock();
		if (radio_button != selected_radio_button) {
			radio_button->SetUnselected();
		}
	}
}


}