#include <Windows.h>
#include <zaf/application.h>
#include <zaf/base/error.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include <zaf/control/list_box.h>
#include <zaf/control/button.h>
#include <zaf/control/layout/array_layouter.h>

void BeginRun(zaf::Application&);

int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
) {

    auto& application = zaf::Application::GetInstance();
    application.GetBeginRunEvent().AddListener(BeginRun);

    std::error_code error_code;
    application.Initialize(error_code);

    if (zaf::IsSucceeded(error_code)) {
        application.Run();
    }
}


void BeginRun(zaf::Application& application) {

    auto window = zaf::Create<zaf::Window>();
    window->SetTitle(L"zaf playground");
    window->SetRect(zaf::Rect(200, 200, 500, 500));

    window->Show();
    application.SetMainWindow(window);

    auto list_box = zaf::Create<zaf::ListBox>();
    for (int i = 0; i != 100; ++i) {
        list_box->AddItemWithText(std::to_wstring(i));
    }

    auto button = zaf::Create<zaf::Button>();
    button->SetRect(zaf::Rect(10, 10, 100, 30));
    button->SetText(L"1");

    auto button2 = zaf::Create<zaf::Button>();
    button2->SetRect(zaf::Rect(10, 50, 100, 30));
    button2->SetText(L"2");

    //window->GetRootControl()->SetLayouter(zaf::GetVerticalArrayLayouter());
    //window->GetRootControl()->AddChild(list_box);
    window->GetRootControl()->AddChild(button);
    window->GetRootControl()->AddChild(button2);
}