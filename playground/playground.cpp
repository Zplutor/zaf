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

        Disposables() += this->MessageHandledEvent().Subscribe([this](const zaf::MessageHandledInfo& info) {
            if (info.Message().ID() == WM_WINDOWPOSCHANGED) {
                auto pos = (WINDOWPOS*)info.Message().LParam();
                zaf::Rect new_rect{ (float)pos->x, (float)pos->y, (float)pos->cx, (float)pos->cy };
                new_rect = zaf::ToDIPs(new_rect, this->GetDPI());
                ZAF_LOG() << "WindowPosChanged: " << new_rect.ToString();
            }
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

    auto& application = zaf::Application::Instance();
    application.Disposables() += application.BeginRunEvent().Subscribe(BeginRun);

    application.Initialize({});

    application.Run();
}

std::shared_ptr<zaf::WindowHolder> holder;

void BeginRun(const zaf::BeginRunInfo& event_info) {

    auto window = zaf::Create<Window>();
    window->SetIsSizable(true);
    window->SetHasTitleBar(true);

    window->SetRect({ 100.25, 100.25, 200.25, 200.25 });
    window->Show();

    auto screens = zaf::ScreenManager::Instance().AllScreens();
    for (const auto& each_screen : screens) {

        ZAF_LOG() << std::format(L"{}, {}", each_screen->Name(), each_screen->Size().ToString());
    }


    zaf::Application::Instance().SetMainWindow(window);
}