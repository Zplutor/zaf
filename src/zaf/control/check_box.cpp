#include <zaf/control/check_box.h>
#include <zaf/base/as.h>
#include <zaf/base/error/check.h>
#include <zaf/base/event_utility.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/geometry/geometry_sink.h>
#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/internal/paint_utility.h>
#include <zaf/internal/theme.h>
#include <zaf/object/object_type.h>
#include <zaf/object/type_definition.h>
#include <zaf/rx/subject.h>
#include <zaf/serialization/properties.h>

namespace zaf {
namespace {

const wchar_t* const kBoxBackgroundColorPickerPropertyName = L"BoxBackgroundColorPicker";
const wchar_t* const kBoxBorderColorPickerPropertyName = L"BoxBorderColorPicker";
const wchar_t* const AutoCheckPropertyName = L"AutoCheck";
const wchar_t* const kCanBeIndeterminatePropertyName = L"CanBeIndeterminate";
const wchar_t* const CheckStateChangedEventPropertyName = L"CheckStateChangedEvent";

}


ZAF_DEFINE_TYPE(CheckBox)
ZAF_DEFINE_TYPE_PROPERTY(AutoCheck)
ZAF_DEFINE_TYPE_PROPERTY(BoxBackgroundColor)
ZAF_DEFINE_TYPE_PROPERTY(BoxBorderColor)
ZAF_DEFINE_TYPE_PROPERTY(CanBeIndeterminate)
ZAF_DEFINE_TYPE_PROPERTY(CheckState)
ZAF_DEFINE_TYPE_PROPERTY(IsChecked)
ZAF_DEFINE_TYPE_END


CheckBox::CheckBox() : check_state_(CheckState::Unchecked) {

}


CheckBox::~CheckBox() {

}


void CheckBox::Initialize() {

    __super::Initialize();

    SetBackgroundColorPicker([](const Control&) {
        return Color::Transparent();
    });
}


void CheckBox::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {
    
    __super::Paint(canvas, dirty_rect);

    Canvas::StateGuard state_guard(canvas);

    auto text_layout = GetTextLayout();
    if (text_layout == nullptr) {
        return;
    }

    internal::DrawTextWithIcon(
        canvas,
        *this,
        text_layout, 
        12,
        std::bind(&CheckBox::PaintBox, this, std::placeholders::_1, std::placeholders::_2));
}


void CheckBox::PaintBox(Canvas& canvas, const zaf::Rect& box_rect) const {

    //Paint the box.
    canvas.SetBrushWithColor(BoxBackgroundColor());
    canvas.DrawRectangle(box_rect);

    canvas.SetBrushWithColor(BoxBorderColor());
    canvas.DrawRectangleFrame(box_rect, 1);

    //Paint the check state mark.
    auto check_state = CheckState();

    if (check_state == CheckState::Indeterminate) {
        zaf::Rect mark_rect = box_rect;
        mark_rect.Inflate(-3);
        canvas.DrawRectangle(mark_rect);
    }
    else if (check_state == CheckState::Checked) {

        auto path = canvas.CreatePathGeometry();
        if (path == nullptr) {
            return;
        }

        auto sink = path.Open();
        if (sink == nullptr) {
            return;
        }

        zaf::Rect mark_rect = box_rect;
        mark_rect.Inflate(-2);

        Point start_point(mark_rect.position.x + mark_rect.size.width, mark_rect.position.y);
        Point middle_point(mark_rect.position.x + mark_rect.size.width * 0.4f, mark_rect.position.y + mark_rect.size.height - 1);
        Point end_point(mark_rect.position.x, mark_rect.position.y + mark_rect.size.height * 0.4f);

        sink.BeginFigure(start_point, GeometrySink::BeginFigureOption::Hollow);
        sink.AddLine(middle_point);
        sink.AddLine(end_point);
        sink.EndFigure(GeometrySink::EndFigureOption::Open);
        sink.Close();

        canvas.DrawGeometryFrame(path, 1.5);
    }
}


zaf::Rect CheckBox::GetTextRect() {
    return zaf::Rect();
}


ColorPicker CheckBox::BoxBorderColorPicker() const {

    auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(kBoxBorderColorPickerPropertyName);
    if ( (color_picker != nullptr) && (*color_picker != nullptr) ) {
        return *color_picker;
    }
    else {
        return internal::GetBoxBorderColorPicker();
    }
}


void CheckBox::SetBoxBorderColorPicker(const ColorPicker& color_picker) {

    GetPropertyMap().SetProperty(kBoxBorderColorPickerPropertyName, color_picker);
    NeedRepaint();
}


ColorPicker CheckBox::BoxBackgroundColorPicker() const {

    auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(kBoxBackgroundColorPickerPropertyName);
    if ( (color_picker != nullptr) && (*color_picker != nullptr) ) {
        return *color_picker;
    }
    else {
        return internal::GetBoxBackgroundColorPicker();
    }
}


void CheckBox::SetBoxBackgroundColorPicker(const ColorPicker& color_picker) {

    GetPropertyMap().SetProperty(kBoxBackgroundColorPickerPropertyName, color_picker);
    NeedRepaint();
}


bool CheckBox::AutoCheck() const {

    auto value = GetPropertyMap().TryGetProperty<bool>(AutoCheckPropertyName);
    if (value != nullptr) {
        return *value;
    }
    else {
        return true;
    }
}


void CheckBox::SetAutoCheck(bool auto_check) {
    GetPropertyMap().SetProperty(AutoCheckPropertyName, auto_check);
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

    if (! can_be_ndeterminate && (CheckState() == CheckState::Indeterminate)) {
        SetCheckState(CheckState::Checked);
    }
}


void CheckBox::SetCheckState(zaf::CheckState check_state) {

    if (check_state_ == check_state) {
        return;
    }

    check_state_ = check_state;
    NeedRepaint();

    auto observer = GetEventObserver<CheckStateChangedInfo>(
        GetPropertyMap(),
        CheckStateChangedEventPropertyName);

    if (observer) {
        observer->OnNext(CheckStateChangedInfo{ As<CheckBox>(shared_from_this()) });
    }
}


Observable<CheckStateChangedInfo> CheckBox::CheckStateChangedEvent() {

    return GetEventObservable<CheckStateChangedInfo>(
        GetPropertyMap(), 
        CheckStateChangedEventPropertyName);
}


void CheckBox::OnClick(const ClickInfo& event_info) {

    __super::OnClick(event_info);

    if (! AutoCheck()) {
        return;
    }

    zaf::CheckState new_check_state = CheckState::Unchecked;

    switch (CheckState()) {

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
            ZAF_ALERT();
            break;
    }

    SetCheckState(new_check_state);
}


}