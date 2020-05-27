#include <zaf/application.h>
#include <zaf/creation.h>
#include "main_window.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    auto& application = zaf::Application::GetInstance();

    application.Initialize({});

    application.GetBeginRunEvent().AddListener([](zaf::Application& application) {

        auto main_window = zaf::Create<MainWindow>();
        application.SetMainWindow(main_window);
        main_window->Show();
    });

    application.Run();
    return 0;
}
