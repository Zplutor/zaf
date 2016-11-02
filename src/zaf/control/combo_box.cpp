#include <zaf/control/combo_box.h>
#include <algorithm>
#include <zaf/application.h>
#include <zaf/control/button.h>
#include <zaf/control/layout/array_layouter.h>
#include <zaf/control/list_box.h>
#include <zaf/control/text_box.h>
#include <zaf/creation.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/clear_edge.h>
#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/internal/theme.h>
#include <zaf/window/message/message.h>
#include <zaf/window/window.h>

#ifdef min
#undef min
#endif

namespace zaf {

static const wchar_t* const kDropDownButtonColorPickerPropertyName = L"DropDownButtonColorPicker";
static const wchar_t* const kDropDownButtonWidthPropertyName = L"DropDownButtonWidth";
static const wchar_t* const kMaximumVisibleItemCountPropertyName = L"MaximumVisibleItemCount";
static const wchar_t* const kMinimumVisibleItemCountPropertyName = L"MinimumVisibleItemCount";

class DropDownWindow : public Window {
public:
    class Container : public Control {
    protected:
        void MouseUp(const Point& position, const MouseMessage& message) override {

            if (! GetRect().Contain(position)) {
                ReleaseMouse();
            }
        }
    };

    void Initialize() override {

        __super::Initialize();

        container_ = Create<Container>();
        container_->SetLayouter(GetVerticalArrayLayouter());

        auto root_control = GetRootControl();
        root_control->SetLayouter(GetVerticalArrayLayouter());
        root_control->AddChild(container_);
    }

    void SetListBox(const std::shared_ptr<ListBox>& list_box) {

        if (list_box_ != nullptr) {
            container_->RemoveChild(list_box_);
        }

        list_box_ = list_box;
        container_->AddChild(list_box_);
    }

protected:
    void WindowShow() override {
        container_->CaptureMouse();
    }

    void CapturingMouseControlChange(const std::shared_ptr<Control>& previous_control) override {

        if (GetCapturingMouseControl() == nullptr) {

            if (previous_control == container_) {
                Close();
            }
            else {
                container_->CaptureMouse();
            }
        }
    }

private:
    std::shared_ptr<Control> container_;
    std::shared_ptr<ListBox> list_box_;
};

void ComboBox::Initialize() {

    __super::Initialize();

    SetBorderThickness(1);

    SetBackgroundColorPicker([](const Control& control) {

        if (! control.IsEnabled()) {
            return Color::FromRGB(internal::ControlBackgroundColorRGB);
        }

        return Color::FromRGB(internal::ControlContentColorRGB);
    });

    SetBorderColorPicker([](const Control& control) {

        const auto& combo_box = dynamic_cast<const ComboBox&>(control);

        if (combo_box.IsPressed() || combo_box.IsHovered()) {
            return Color::FromRGB(internal::ButtonActivedBorderColorRGB);
        }

        return Color::Black;
    });

    drop_down_list_ = Create<ListBox>();
    drop_down_list_->SetAllowHorizontalScroll(false);
    drop_down_list_->SetAutoHideScrollBars(true);

    auto drop_down_window = Create<DropDownWindow>();
    drop_down_window->SetActivateOption(Window::ActivateOption::NoActivate);
    drop_down_window->SetBorderStyle(Window::BorderStyle::None);
    drop_down_window->SetInitialRectStyle(Window::InitialRectStyle::Custom);
    drop_down_window->SetListBox(drop_down_list_);
    drop_down_window_ = drop_down_window;
}


void ComboBox::Paint(Canvas& canvas, const Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    float button_width = GetDropDownButtonWidth();
    auto button_rect = GetContentRect();
    button_rect.position.x = button_rect.position.x + button_rect.size.width - button_width;
    button_rect.size.width = button_width;

    float top_edge_length = button_width / 2;
    if (top_edge_length / 2 > button_rect.size.height / 2) {
        top_edge_length = button_rect.size.height / 2;
    }

    float height = top_edge_length / 2;
    float half_height = height / 2;
    Point center_point(
        button_rect.position.x + button_rect.size.width / 2,
        button_rect.position.y + button_rect.size.height / 2);

    Point left_point(center_point.x - height, center_point.y - half_height);
    left_point = MakeClearEdgePointForFill(left_point, ClearEdgeOption::Clear);

    Point right_point(center_point.x + height, center_point.y - half_height);
    right_point = MakeClearEdgePointForFill(right_point, ClearEdgeOption::Clear);

    Point bottom_point(center_point.x, center_point.y + half_height);
    bottom_point = MakeClearEdgePointForFill(bottom_point, ClearEdgeOption::Clear);

    auto path = GetApplication().GetResourceFactory()->CreatePathGeometry();
    if (path == nullptr) {
        return;
    }

    auto sink = path->Open();
    if (sink == nullptr) {
        return;
    }

    sink->BeginFigure(left_point, GeometrySink::BeginFigureOption::Fill);
    sink->AddLine(right_point);
    sink->AddLine(bottom_point);
    sink->EndFigure(GeometrySink::EndFigureOption::Close);
    sink->Close();

    Canvas::StateGuard state_guard(canvas);
    canvas.SetBrushWithColor(GetDropDownButtonColor());
    canvas.DrawGeometry(path);
}


const ColorPicker ComboBox::GetDropDownButtonColorPicker() const {

    auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(kDropDownButtonColorPickerPropertyName);
    if (color_picker != nullptr) {
        return *color_picker;
    }
    else {
        return [](const Control&) { return Color::Black; };
    }
}

void ComboBox::SetDropDownButtonColorPicker(const ColorPicker& color_picker) {

    GetPropertyMap().SetProperty(kDropDownButtonColorPickerPropertyName, color_picker);
    NeedRepaint();
}


float ComboBox::GetDropDownButtonWidth() const {

    auto width = GetPropertyMap().TryGetProperty<float>(kDropDownButtonWidthPropertyName);
    if (width != nullptr) {
        return *width;
    }
    else {
        return 12;
    }
}

void ComboBox::SetDropDownButtonWidth(float width) {

    GetPropertyMap().SetProperty(kDropDownButtonWidthPropertyName, width);
    NeedRepaint();
}


std::size_t ComboBox::GetMinimumVisibleItemCount() const {

    auto count = GetPropertyMap().TryGetProperty<std::size_t>(kMinimumVisibleItemCountPropertyName);
    if (count != nullptr) {
        return *count;
    }
    else {
        return 1;
    }
}

void ComboBox::SetMinimumVisibleItemCount(std::size_t count) {
    GetPropertyMap().SetProperty(kMinimumVisibleItemCountPropertyName, count);
}


std::size_t ComboBox::GetMaximumVisibleItemCount() const {

    auto count = GetPropertyMap().TryGetProperty<std::size_t>(kMaximumVisibleItemCountPropertyName);
    if (count != nullptr) {
        return *count;
    }
    else {
        return std::numeric_limits<std::size_t>::max();
    }
}

void ComboBox::SetMaximumVisibleItemCount(std::size_t count) {
    GetPropertyMap().SetProperty(kMaximumVisibleItemCountPropertyName, count);
}


void ComboBox::SetDropDownList(const std::shared_ptr<ListBox>& list) {

}


void ComboBox::SetEditTextBox(const std::shared_ptr<TextBox>& text_box) {


}


void ComboBox::MouseClick() {
    PopupDropDownWindow();
}


void ComboBox::PopupDropDownWindow() {

    auto window = GetWindow();
    if (window == nullptr) {
        return;
    }

    auto window_rect = GetAbsoluteRect();
    window_rect.Inflate(-1, 0);
    window_rect.position.y += GetHeight() - 2;

    std::size_t visible_item_count = drop_down_list_->GetItemCount();
    visible_item_count = std::max(visible_item_count, GetMinimumVisibleItemCount());
    visible_item_count = std::min(visible_item_count, GetMaximumVisibleItemCount());

    window_rect.size.height =
        visible_item_count * drop_down_list_->GetItemHeight() + drop_down_list_->GetBorderThickness() * 2;

    POINT screen_position = window_rect.position.ToPOINT();
    ClientToScreen(window->GetHandle(), &screen_position);
    window_rect.position = Point::FromPOINT(screen_position);

    drop_down_window_->SetOwner(window);
    drop_down_window_->SetRect(window_rect);
    drop_down_window_->Show();
}

}