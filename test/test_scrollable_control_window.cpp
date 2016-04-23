#include <zaf/zaf.h>

using namespace zaf;

class TestScrollableControlWindow : public Window {
public:
    void Initialize() {

        SetTitle(L"test scrollable control");
        SetRect(Rect(0, 0, 500, 500));

        auto scrollable_control = CreateControl<ScrollableControl>();
        scrollable_control->SetRect(Rect(50, 0, 300, 300));
        scrollable_control->SetScrollAreaSize(Size(500, 500));
        scrollable_control->SetScrolledControl(CreateGeneralScrolledControl());
        GetRootControl()->AddChild(scrollable_control);
    }

private:
    std::shared_ptr<Control> CreateGeneralScrolledControl() {

        auto scrolled_control = CreateControl<Control>();

        auto left_top_corner_button = CreateControl<Button>();
        left_top_corner_button->SetText(L"LeftTop");
        left_top_corner_button->SetRect(Rect(0, 0, 100, 100));
        scrolled_control->AddChild(left_top_corner_button);

        auto right_top_corner_button = CreateControl<Button>();
        right_top_corner_button->SetText(L"RightTop");
        right_top_corner_button->SetRect(Rect(400, 0, 100, 100));
        scrolled_control->AddChild(right_top_corner_button);

        auto left_bottom_corner_button = CreateControl<Button>();
        left_bottom_corner_button->SetText(L"LeftBottom");
        left_bottom_corner_button->SetRect(Rect(0, 400, 100, 100));
        scrolled_control->AddChild(left_bottom_corner_button);

        auto right_bottom_corner_button = CreateControl<Button>();
        right_bottom_corner_button->SetText(L"RightBottom");
        right_bottom_corner_button->SetRect(Rect(400, 400, 100, 100));
        scrolled_control->AddChild(right_bottom_corner_button);

        return scrolled_control;
    }
};

void ShowTestScrollableControlWindow() {

    auto window = std::make_shared<TestScrollableControlWindow>();
    window->Initialize();
    window->Show();
}