#include <Windows.h>
#include <zaf/application.h>
#include <zaf/base/error.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include <zaf/window/dialog.h>
#include <zaf/control/list_box.h>
#include <zaf/control/button.h>
#include <zaf/control/layout/array_layouter.h>
#include <zaf/control/text_box.h>
#include <zaf/window/message/hit_test_result.h>
#include <zaf/graphic/canvas.h>
#include <zaf/control/combo_box.h>

void BeginRun(zaf::Application&);


class RootControl : public zaf::Control {
public:
    void Initialize() override {
        __super::Initialize();

        close_button_ = zaf::Create<zaf::Button>();
        close_button_->SetSize(zaf::Size(30, 30));
        close_button_->SetPosition(zaf::Point(0, 0));
        close_button_->SetText(L"X");
        close_button_->GetClickEvent().AddListener(std::bind([this]() {
            auto window = GetWindow();
            if (window != nullptr) {
                window->Maximize();
            }
        }));

        auto button = zaf::Create<zaf::Button>();
        button->SetSize(zaf::Size(100, 30));
        button->SetPosition(zaf::Point(0, 30));
        button->SetText(L"Button");
        button->GetClickEvent().AddListener(std::bind([this]() {
            auto window = GetWindow();
            if (window != nullptr) {
                window->Restore();
            }
        }));

        auto combo_box = zaf::Create<zaf::ComboBox>();
        combo_box->SetRect(zaf::Rect(0.3, 80.3, 200, 30));
        AddChild(combo_box);

        AddChild(close_button_);
        AddChild(button);
    }

    void Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) override {

        __super::Paint(canvas, dirty_rect);

        canvas.SetBrushWithColor(zaf::Color::Blue);
        canvas.DrawRectangleFrame(zaf::Rect(80, 100, 100, 100), 3);
    }

    std::optional<zaf::HitTestResult> HitTest(const zaf::HitTestMessage& message) override {

        zaf::Rect title_bar_rect(0, 0, GetWidth(), 30);

        if (close_button_->GetRect().Contain(close_button_->GetMousePosition())) {
            return zaf::HitTestResult::CloseButton;
        }

        if (title_bar_rect.Contain(GetMousePosition())) {
            return zaf::HitTestResult::TitleBar;
        }

        zaf::Rect left_rect(80, 100, 3, 100);
        if (left_rect.Contain(GetMousePosition())) {
            return zaf::HitTestResult::LeftBorder;
        }

        return __super::HitTest(message);
    }

private:
    std::shared_ptr<zaf::Button> close_button_;
};


int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
) {

    auto& application = zaf::Application::GetInstance();
    application.GetBeginRunEvent().AddListener(BeginRun);

    std::error_code error_code;
    application.Initialize(error_code);

    if (zaf::IsSucceeded(error_code)) {
        application.Run();
    }
}


void BeginRun(zaf::Application& application) {

    auto window = zaf::Create<zaf::Window>();

    auto root_control = zaf::Create<RootControl>();
    window->SetBorderStyle(zaf::Window::BorderStyle::None);
    window->SetRootControl(root_control);
    window->Show();

    application.SetMainWindow(window);
}