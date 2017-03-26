#include <Windows.h>
#include <zaf/application.h>
#include <zaf/base/error.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

#include <zaf/control/layout/array_layouter.h>
#include <zaf/control/image_box.h>
#include <zaf/graphic/image/image_decoder.h>
#include <zaf/graphic/resource_factory.h>

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

    auto child_control = zaf::Create<zaf::Control>();
    child_control->SetRect(zaf::Rect(-50, -50, 100, 100));
    child_control->SetBackgroundColor(zaf::Color::Red);

    auto parent_control = zaf::Create<zaf::Control>();
    parent_control->SetRect(zaf::Rect(100, 100, 100, 100));
    parent_control->AddChild(child_control);
    parent_control->SetBackgroundColor(zaf::Color::Blue);

    window->GetRootControl()->AddChild(parent_control);

    /**
    auto decoder = zaf::GetResourceFactory()->CreateImageDecoder(L"C:\\Users\\Zplutor\\Desktop\\zaf\\gif\\5.gif");

    auto image_box = zaf::Create<zaf::ImageBox>();
    image_box->SetImageDecoder(decoder);

    auto root_control = window->GetRootControl();
    root_control->SetLayouter(zaf::GetHorizontalArrayLayouter());
    root_control->AddChild(image_box);
    */

    window->Show();
    application.SetMainWindow(window);
}