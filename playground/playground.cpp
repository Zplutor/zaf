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

void BeginRun(const zaf::BeginRunInfo& event_info);

class Window : public zaf::Window {
protected:
    void Initialize() override {

        __super::Initialize();

        Disposables() += this->DPIChangedEvent().Subscribe([](const zaf::DPIChangedInfo&) {
            ZAF_LOG() << "DPI Changed.";
        });

        Disposables() += this->SizeChangedEvent().Subscribe([](const zaf::WindowSizeChangedInfo& event_info) {
            ZAF_LOG() << "Size Changed.";
        });
    }

private:
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