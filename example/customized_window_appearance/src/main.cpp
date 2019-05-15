#include <Windows.h>
#include <zaf/application.h>
#include <zaf/window/window.h>
#include <zaf/creation.h>
#include "root_control.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    zaf::Application& application = zaf::GetApplication();

    std::error_code error_code;
    application.Initialize(error_code);
    if (error_code) {
        return error_code.value();
    }

    application.GetBeginRunEvent().AddListener([](zaf::Application& application) {
    
        auto window = zaf::Create<zaf::Window>();

        auto root_control = zaf::Create<RootControl>();
        window->SetRootControl(root_control);
        window->Show();

        application.SetMainWindow(window);
    });

    application.Run();
    return 0;
}