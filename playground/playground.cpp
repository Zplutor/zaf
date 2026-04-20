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

void BeginRun(const zaf::ApplicationStartedInfo& event_info);

class Window : public zaf::Window {
protected:
    void Initialize() override {

        __super::Initialize();

        auto list_box = zaf::Create<zaf::ListBox>();
        list_box->AddItem(zaf::Box("AAA"));
        list_box->AddItem(zaf::Box("BBB"));
        list_box->AddItem(zaf::Box("CCC"));
        list_box->AddItem(zaf::Box("DDD"));
        list_box->SetDefaultTextColorPicker(zaf::ColorPicker([](const zaf::Object&){ 
            return zaf::Color::Red(); 
        }));
        list_box->SetDefaultBackgroundColorPicker(zaf::ColorPicker([](const zaf::Object&) {
            return zaf::Color::Green();
        }));

        this->RootControl()->SetLayouter(zaf::Create<zaf::VerticalLayouter>());
        this->RootControl()->AddChild(list_box);
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
    window->Show();
    zaf::Application::Instance().SetMainWindow(window);
}