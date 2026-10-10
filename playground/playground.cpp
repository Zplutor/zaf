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
#include <zaf/window/tray_icon.h>
#include <zaf/window/popup_menu.h>
#include <zaf/control/list_box.h>
#include <zaf/graphic/canvas/canvas.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/graphic/d2d/stroke_style_properties.h>

void BeginRun(const zaf::ApplicationStartedInfo& event_info);

class Window : public zaf::Window {
protected:
    void Initialize() override {

        __super::Initialize();

        auto button = zaf::Create<zaf::Button>();
        button->SetTooltip(L"Pause");

        this->RootControl()->SetLayouter(zaf::Create<zaf::VerticalLayouter>());
        this->RootControl()->AddChild(button);
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
    application.Disposables() += application.StartedEvent().Subscribe(BeginRun);

    application.Initialize({});

    application.Run();
}


void BeginRun(const zaf::ApplicationStartedInfo& event_info) {

    auto window = zaf::Create<Window>();
    window->SetTitle(L"New Canvas");
    window->Show();
    zaf::Application::Instance().SetMainWindow(window);
}