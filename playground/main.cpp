#include <Windows.h>
#include <zaf/application.h>
#include <zaf/base/error.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include <zaf/window/dialog.h>
#include <zaf/control/list_box.h>
#include <zaf/control/button.h>
#include <zaf/control/text_box.h>
#include <zaf/window/message/hit_test_result.h>
#include <zaf/graphic/canvas.h>
#include <zaf/control/combo_box.h>
#include <zaf/reflection/reflection_type.h>
#include <zaf/reflection/reflection_manager.h>
#include <zaf/parsing/helpers.h>
#include <zaf/parsing/xaml_reader.h>
#include <zaf/reflection/creation.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/label.h>

void BeginRun(zaf::Application&);


class RootControl : public zaf::Control {
public:
    void Initialize() override {
        __super::Initialize();
    }

    void Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) override {

        __super::Paint(canvas, dirty_rect);
    }
};


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

    auto root_control = zaf::Create<RootControl>();
    window->SetBorderStyle(zaf::Window::BorderStyle::Normal);
    window->SetCanMaximize(true);
    window->SetIsSizable(true);
    window->SetRootControl(root_control);
    window->Show();

    auto label = zaf::Create<zaf::Label>();
    label->SetMaximumWidth(100);
    label->SetText(L"��playground.exe��(Win32): ��ж�ء�C:\\Windows\\SysWOW64\\prl_umdd10.dll��\n��playground.exe��(Win32): �Ѽ��ء�C:\\Windows\\SysWOW64\\dwmapi.dll����");
    label->SetWordWrapping(zaf::WordWrapping::Wrap);
    label->ResizeToPreferredSize();
    root_control->AddChild(label);

    application.SetMainWindow(window);
}