#include <zaf/application.h>
#include <zaf/creation.h>
#include "logic/service.h"
#include "ui/main/main_window.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    auto& application = zaf::Application::Instance();

    application.Initialize({});

    application.Disposables() += application.StartedEvent().Subscribe(
        [](const zaf::ApplicationStartedInfo&) {

            Service::GetInstance().Initialize();

            auto main_window = zaf::Create<MainWindow>();
            main_window->Show();
        
            zaf::Application::Instance().SetMainWindow(main_window);
        }
    );

    application.Run();
    return 0;
}
