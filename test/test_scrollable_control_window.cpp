#include <zaf/zaf.h>

using namespace zaf;

class TestScrollableControlWindow : public Window {
public:
    void Initialize() {

        __super::Initialize();

        SetTitle(L"test scrollable control");
        SetRect(Rect(0, 0, 500, 500));

        scrollable_control_ = Create<ScrollableControl>();
        scrollable_control_->SetRect(Rect(0, 0, 300, 300));
        scrollable_control_->SetBorderWidth(2);
        scrollable_control_->SetBorderColorPicker([](const Control&) { return Color::Black; });
        scrollable_control_->SetScrollAreaSize(Size(500, 500));
        scrollable_control_->SetAnchor(Anchor::Left | Anchor::Top | Anchor::Right);
        scrollable_control_->SetScrolledControl(CreateGeneralScrolledControl());
        GetRootControl()->AddChild(scrollable_control_);

        auto options_container = CreateOptionsContainerControl();
        options_container->SetRect(Rect(0, 300, 500, 200));
        GetRootControl()->AddChild(options_container);
    }

private:
    std::shared_ptr<Control> CreateGeneralScrolledControl() {

        auto scrolled_control = Create<Control>();

        auto left_top_corner_button = Create<Button>();
        left_top_corner_button->SetText(L"LeftTop");
        left_top_corner_button->SetRect(Rect(0, 0, 100, 100));
        scrolled_control->AddChild(left_top_corner_button);

        auto right_top_corner_button = Create<Button>();
        right_top_corner_button->SetText(L"RightTop");
        right_top_corner_button->SetRect(Rect(400, 0, 100, 100));
        scrolled_control->AddChild(right_top_corner_button);

        auto left_bottom_corner_button = Create<Button>();
        left_bottom_corner_button->SetText(L"LeftBottom");
        left_bottom_corner_button->SetRect(Rect(0, 400, 100, 100));
        scrolled_control->AddChild(left_bottom_corner_button);

        auto right_bottom_corner_button = Create<Button>();
        right_bottom_corner_button->SetText(L"RightBottom");
        right_bottom_corner_button->SetRect(Rect(400, 400, 100, 100));
        scrolled_control->AddChild(right_bottom_corner_button);

        return scrolled_control;
    }

    std::shared_ptr<Control> CreateOptionsContainerControl() {

        auto container = Create<Control>();
        container->SetLayouter(GetVerticalArrayLayouter());

        auto allow_vertical_scroll_check_box = Create<CheckBox>();
        allow_vertical_scroll_check_box->SetText(L"Allow vertical scroll");
        allow_vertical_scroll_check_box->SetIsChecked(scrollable_control_->AllowVerticalScroll());
        allow_vertical_scroll_check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
            scrollable_control_->SetAllowVerticalScroll(check_box->IsChecked());
        });
        container->AddChild(allow_vertical_scroll_check_box);

        auto allow_horizontal_scroll_check_box = Create<CheckBox>();
        allow_horizontal_scroll_check_box->SetText(L"Allow horizontal scroll");
        allow_horizontal_scroll_check_box->SetIsChecked(scrollable_control_->AllowHorizontalScroll());
        allow_horizontal_scroll_check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
            scrollable_control_->SetAllowHorizontalScroll(check_box->IsChecked());
        });
        container->AddChild(allow_horizontal_scroll_check_box);

        auto auto_hide_scroll_bars_check_box = Create<CheckBox>();
        auto_hide_scroll_bars_check_box->SetText(L"Auto hide scroll bars");
        auto_hide_scroll_bars_check_box->SetIsChecked(scrollable_control_->AutoHideScrollBars());
        auto_hide_scroll_bars_check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
            scrollable_control_->SetAutoHideScrollBars(check_box->IsChecked());
        });
        container->AddChild(auto_hide_scroll_bars_check_box);

        return container;
    }

private:
    std::shared_ptr<ScrollableControl> scrollable_control_;
};

void ShowTestScrollableControlWindow() {

    auto window = Create<TestScrollableControlWindow>();
    window->Show();
}