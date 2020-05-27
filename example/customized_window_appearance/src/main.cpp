#include <Windows.h>
#include <zaf/application.h>
#include <zaf/window/window.h>
#include <zaf/creation.h>
#include "root_control.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    zaf::Application& application = zaf::GetApplication();

    application.Initialize({});
    
    application.GetBeginRunEvent().AddListener([](zaf::Application& application) {
    
        auto window = zaf::Create<zaf::Window>();

        auto root_control = zaf::Create<RootControl>();
        window->SetBorderStyle(zaf::Window::BorderStyle::None);
        window->SetRootControl(root_control);
        window->Show();

        application.SetMainWindow(window);
    });

    application.Run();
    return 0;
}