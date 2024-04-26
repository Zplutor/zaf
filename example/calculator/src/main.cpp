#include <Windows.h>
#include <zaf/application.h>
#include <zaf/creation.h>
#include "ui/main_window.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    auto& application = zaf::Application::Instance();
    
    application.Initialize({});

    application.Subscriptions() += application.BeginRunEvent().Subscribe(
        [](const zaf::BeginRunInfo&) {

            auto main_window = zaf::Create<MainWindow>();
            zaf::Application::Instance().SetMainWindow(main_window);
            main_window->Show();
        }
    );

    application.Run();
    return 0;
}
