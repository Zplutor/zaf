#include <zaf/control/check_box.h>
#include <zaf/base/as.h>
#include <zaf/base/error/check.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/geometry/geometry_sink.h>
#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/internal/paint_utility.h>
#include <zaf/internal/theme.h>
#include <zaf/object/object_type.h>
#include <zaf/rx/subject.h>

namespace zaf {

ZAF_OBJECT_IMPL(CheckBox);


CheckBox::CheckBox() : check_state_(CheckState::Unchecked) {

}


CheckBox::~CheckBox() {

}


void CheckBox::Initialize() {

    __super::Initialize();

    SetBackgroundColor(Color::Transparent());

    SetBoxBorderColorPicker(ColorPicker([](const Control& control) {
        return internal::GetBoxBorderColor(As<ClickableControl>(control));
    })); 

    SetBoxBackColorPicker(ColorPicker([](const Control& control) {
        return internal::GetBoxBackgroundColor(As<ClickableControl>(control));
    })); 
}


zaf::Size CheckBox::CalculatePreferredContentSize(const zaf::Size& max_size) const {

    auto extra_icon_width = internal::IconSize + internal::IconMargin;

    auto revised_max_size = max_size;
    revised_max_size.width -= extra_icon_width;

    auto result = __super::CalculatePreferredContentSize(revised_max_size);
    result.width += extra_icon_width;
    return result;
}


void CheckBox::UpdateStyle() {

    __super::UpdateStyle();

    box_back_color_field_.UpdateColor(*this);
    box_border_color_field_.UpdateColor(*this);
}


void CheckBox::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const {
    
    __super::Paint(canvas, dirty_rect);

    auto state_guard = canvas.PushState();

    auto text_layout = GetTextLayout();

    internal::DrawTextWithIcon(
        canvas,
        *this,
        text_layout, 
        std::bind(&CheckBox::PaintBox, this, std::placeholders::_1, std::placeholders::_2));
}


void CheckBox::PaintBox(Canvas& canvas, const zaf::Rect& box_rect) const {

    //Paint the box.
    canvas.SetBrushWithColor(BoxBackColor());
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
        if (!path) {
            return;
        }

        auto sink = path.Open();
        if (!sink) {
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


zaf::Rect CheckBox::DetermineTextRect() {
    return zaf::Rect();
}


Color CheckBox::BoxBorderColor() const {
    return box_border_color_field_.Color();
}


void CheckBox::SetBoxBorderColor(const Color& color) {
    box_border_color_field_.SetColor(color, *this);
}


const ColorPicker& CheckBox::BoxBorderColorPicker() const {
    return box_border_color_field_.ColorPicker();
}

void CheckBox::SetBoxBorderColorPicker(ColorPicker picker) {
    box_border_color_field_.SetColorPicker(std::move(picker), *this);
}


Color CheckBox::BoxBackColor() const {
    return box_back_color_field_.Color();
}

void CheckBox::SetBoxBackColor(const Color& color) {
    box_back_color_field_.SetColor(color, *this);
}


const ColorPicker& CheckBox::BoxBackColorPicker() const {
    return box_back_color_field_.ColorPicker();
}

void CheckBox::SetBoxBackColorPicker(ColorPicker picker) {
    box_back_color_field_.SetColorPicker(std::move(picker), *this);
}


bool CheckBox::AutoCheck() const {
    return auto_check_;
}


void CheckBox::SetAutoCheck(bool auto_check) {
    auto_check_ = auto_check;
}


bool CheckBox::CanBeIndeterminate() const {
    return can_be_indeterminate_;
}


void CheckBox::SetCanBeIndeterminate(bool can_be_indeterminate) {

    can_be_indeterminate_ = can_be_indeterminate;

    if (!can_be_indeterminate && (CheckState() == CheckState::Indeterminate)) {
        SetCheckState(CheckState::Checked);
    }
}


void CheckBox::SetCheckState(zaf::CheckState check_state) {

    if (check_state_ == check_state) {
        return;
    }

    check_state_ = check_state;
    NeedRepaint();

    check_state_changed_event_.Raise(CheckStateChangedInfo{ As<CheckBox>(shared_from_this()) });
}


Observable<CheckStateChangedInfo> CheckBox::CheckStateChangedEvent() const {
    return check_state_changed_event_.GetObservable();
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