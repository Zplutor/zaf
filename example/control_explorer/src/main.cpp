#include <zaf/application.h>
#include <zaf/base/error.h>
#include <zaf/creation.h>
#include "main_window.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    auto& application = zaf::Application::GetInstance();

    std::error_code error_code;
    application.Initialize(error_code);

    if (! zaf::IsSucceeded(error_code)) {
        return error_code.value();
    }

    application.GetBeginRunEvent().AddListener([](zaf::Application& application) {

        auto main_window = zaf::Create<MainWindow>();
        application.SetMainWindow(main_window);
        main_window->Show();
    });

    application.Run();
    return 0;
}
