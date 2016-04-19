#include <zaf/control/check_box.h>
#include <zaf/base/assert.h>
#include <zaf/control/paint_utility.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/geometry/geometry_sink.h>
#include <zaf/graphic/geometry/path_geometry.h>

namespace zaf {

static const wchar_t* const kBoxColorPickerPropertyName = L"BoxColorPicker";
static const wchar_t* const kCanAutoChangeCheckStatePropertyName = L"CanAutoChangeCheckState";
static const wchar_t* const kCanBeIndeterminatePropertyName = L"CanBeIndeterminate";
static const wchar_t* const kCheckStateChangeEventPropertyName = L"CheckStateChangeEvent";

CheckBox::CheckBox() : check_state_(CheckState::Unchecked) {

}


CheckBox::~CheckBox() {

}


void CheckBox::Initialize() {

	__super::Initialize();

	SetBoxColorPicker([](const Control& control) {
	
		const auto& check_box = dynamic_cast<const CheckBox&>(control);

		if (! check_box.IsEnabled()) {
			return Color::Gray;
		}

		if (check_box.IsPressed()) {
			return Color::FromRGB(0x0000CD);
		}

		if (check_box.IsHovered()) {
			return Color::FromRGB(0x4169E1);
		}

		if (check_box.IsFocused()) {
			return Color::FromRGB(0x0000CD);
		}

		return Color::Black;
	});
}


void CheckBox::Paint(Canvas& canvas, const Rect& dirty_rect) {
	
	__super::Paint(canvas, dirty_rect);

	Canvas::StateGuard state_guard(canvas);

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
			canvas.SetBrushWithColor(GetTextColor());
			canvas.DrawText(text_layout, text_rect.position);
		},
		std::bind(&CheckBox::PaintBox, this, std::placeholders::_1, std::placeholders::_2)
	);
}


void CheckBox::PaintBox(Canvas& canvas, const Rect& box_rect) const {

	//Paint the box frame.
	canvas.SetBrushWithColor(GetBoxColor());
	canvas.DrawRectangleFrame(box_rect, 1);

	//Paint the check state mark.
	auto check_state = GetCheckState();

	if (check_state == CheckState::Indeterminate) {
		Rect mark_rect = box_rect;
		mark_rect.Inflate(-3);
		canvas.DrawRectangle(mark_rect);
	}
	else if (check_state == CheckState::Checked) {

		auto path = canvas.CreatePathGeometry();
		if (path == nullptr) {
			return;
		}

		auto sink = path->Open();
		if (sink == nullptr) {
			return;
		}

		Rect mark_rect = box_rect;
		mark_rect.Inflate(-2);

		Point start_point(mark_rect.position.x + mark_rect.size.width, mark_rect.position.y);
		Point middle_point(mark_rect.position.x + mark_rect.size.width * 0.4f, mark_rect.position.y + mark_rect.size.height - 1);
		Point end_point(mark_rect.position.x, mark_rect.position.y + mark_rect.size.height * 0.4f);

		sink->BeginFigure(start_point, GeometrySink::BeginFigureOption::Hollow);
		sink->AddLine(middle_point);
		sink->AddLine(end_point);
		sink->EndFigure(GeometrySink::EndFigureOption::Open);
		sink->Close();

		canvas.DrawGeometryFrame(path, 1.5);
	}
}


const Rect CheckBox::GetTextRect() const {
    return Rect();
}


const ColorPicker CheckBox::GetBoxColorPicker() const {

	auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(kBoxColorPickerPropertyName);
	if ( (color_picker != nullptr) && (*color_picker != nullptr) ) {
		return *color_picker;
	}

	return [](const Control&) {
		return Color::Black;
	};
}


void CheckBox::SetBoxColorPicker(const ColorPicker& color_picker) {

	GetPropertyMap().SetProperty(kBoxColorPickerPropertyName, color_picker);
	NeedRepaint();
}


bool CheckBox::CanAutoChangeCheckState() const {

	auto value = GetPropertyMap().TryGetProperty<bool>(kCanAutoChangeCheckStatePropertyName);
	if (value != nullptr) {
		return *value;
	}
	else {
		return true;
	}
}


void CheckBox::SetCanAutoChangeCheckState(bool can_change) {
	GetPropertyMap().SetProperty(kCanAutoChangeCheckStatePropertyName, can_change);
}


bool CheckBox::CanBeIndeterminate() const {

	auto value = GetPropertyMap().TryGetProperty<bool>(kCanBeIndeterminatePropertyName);
	if (value != nullptr) {
		return *value;
	}
	else {
		return false;
	}
}


void CheckBox::SetCanBeIndeterminate(bool can_be_ndeterminate) {

	GetPropertyMap().SetProperty(kCanBeIndeterminatePropertyName, can_be_ndeterminate);

	if (! can_be_ndeterminate && (GetCheckState() == CheckState::Indeterminate)) {
		SetCheckState(CheckState::Checked);
	}
}


void CheckBox::SetCheckState(CheckState check_state) {

	if (check_state_ == check_state) {
		return;
	}

	check_state_ = check_state;
	NeedRepaint();

	auto event = GetPropertyMap().TryGetProperty<CheckStateChangeEvent>(kCheckStateChangeEventPropertyName);
	if (event != nullptr) {
		event->Trigger(std::dynamic_pointer_cast<CheckBox>(shared_from_this()));
	}
}


CheckBox::CheckStateChangeEvent::Proxy CheckBox::GetCheckStateChangeEvent() {

	auto& event = GetPropertyMap().GetProperty<CheckStateChangeEvent>(kCheckStateChangeEventPropertyName);
	return CheckStateChangeEvent::Proxy(event);
}


void CheckBox::MouseClick() {

	if (! CanAutoChangeCheckState()) {
		return;
	}

	CheckState new_check_state = CheckState::Unchecked;

	switch (GetCheckState()) {

		case CheckState::Unchecked:
			new_check_state = CheckState::Checked;
			break;

		case CheckState::Checked:
			new_check_state = CanBeIndeterminate() ? CheckState::Indeterminate : CheckState::Unchecked;
			break;

		case CheckState::Indeterminate:
			new_check_state = CheckState::Unchecked;
			break;

		default:
			ZAF_FAIL();
			break;
	}

	SetCheckState(new_check_state);
}

}