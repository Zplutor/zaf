#include <zaf/application.h>
#include <zaf/creation.h>
#include "logic/service.h"
#include "ui/main/main_window.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    auto& application = zaf::Application::GetInstance();

    application.Initialize({});

    application.GetBeginRunEvent().AddListener([](zaf::Application& application) {

        Service::GetInstance().Initialize();

        auto main_window = zaf::Create<MainWindow>();
        main_window->Show();
        
        application.SetMainWindow(main_window);
    });

    application.Run();
    return 0;
}
