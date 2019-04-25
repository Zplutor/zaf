#include <Windows.h>
#include <zaf/application.h>
#include <zaf/base/error.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include <zaf/control/list_box.h>
#include <zaf/control/button.h>
#include <zaf/control/layout/array_layouter.h>
#include <zaf/control/text_box.h>

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
    window->SetRect(zaf::Rect(200, 200, 800, 800));

    window->Show();
    application.SetMainWindow(window);

    auto parent = zaf::Create<zaf::Control>();
    parent->SetBorder(1);
    parent->SetBorderColor(zaf::Color::Black);
    parent->SetRect(zaf::Rect(50, 50, 300, 300));

    auto text_box = zaf::Create<zaf::TextBox>();
    text_box->SetText(L"0123456789");
    text_box->SetRect(zaf::Rect(-5, -8, 200, 30));
    parent->AddChild(text_box);

    auto button = zaf::Create<zaf::Button>();
    button->SetText(L"button");
    button->SetRect(zaf::Rect(-5, 295, 200, 30));
    parent->AddChild(button); 

    window->GetRootControl()->AddChild(parent);

    auto list_box = zaf::Create<zaf::ListBox>();
    list_box->SetRect(zaf::Rect(50, 370, 200, 100));
    for (int i = 0; i < 100; ++i) {
        list_box->AddItemWithText(std::to_wstring(i));
    }
    window->GetRootControl()->AddChild(list_box);

    //window->GetRootControl()->AddChild(button);
    //window->GetRootControl()->AddChild(button2);
}