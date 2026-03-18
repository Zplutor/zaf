#include <format>
#include <zaf/application.h>
#include <zaf/base/log.h>
#include <zaf/graphic/dpi.h>
#include <zaf/control/button.h>
#include <zaf/window/window.h>
#include <zaf/rx/scheduler/main_thread_scheduler.h>
#include <zaf/rx/scheduler/new_thread_scheduler.h>
#include <zaf/rx/timer.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/window/screen_manager.h>
#include <zaf/control/text_box.h>

void BeginRun(const zaf::BeginRunInfo& event_info);

class Window : public zaf::Window {
protected:
    void Initialize() override {

        __super::Initialize();

        this->SetIsPopup(true);

        text_box_ = zaf::Create<zaf::TextBox>();
        text_box_->SetRect(zaf::Rect{ 0, 0, 200, 100 });
        text_box_->SetFontSize(14);
        text_box_->SetCaretColor(zaf::Color::Red());
        text_box_->SetText(L"TextBox");
        Disposables() += text_box_->TextChangedEvent().Subscribe([this](const zaf::TextChangedInfo& event_info) {
            ZAF_LOG() << L"TextChanged: " << text_box_->Text();
        });
        RootControl()->AddChild(text_box_);

        auto button = zaf::Create<zaf::Button>();
        button->SetRect(zaf::Rect{ 0, 100, 200, 30 });
        RootControl()->AddChild(button);
    }

    void OnShow(const zaf::ShowInfo& event_info) override {
        __super::OnShow(event_info);
        text_box_->SetIsFocused(true);
    }

private:
    std::shared_ptr<zaf::TextBox> text_box_;
};


int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
) {

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    auto& application = zaf::Application::Instance();
    application.Disposables() += application.BeginRunEvent().Subscribe(BeginRun);

    application.Initialize({});

    application.Run();
}

std::shared_ptr<zaf::WindowHolder> holder;

void BeginRun(const zaf::BeginRunInfo& event_info) {

    auto window = zaf::Create<Window>();
    window->Show();
    zaf::Application::Instance().SetMainWindow(window);
}