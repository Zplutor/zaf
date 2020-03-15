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

    auto layouter = zaf::Create<zaf::LinearLayouter>();
    layouter->SetDirection(zaf::LayoutDirection::TopToBottom);
    layouter->SetControlAlignment(zaf::ControlAlignment::Center);
    layouter->SetAxisAlignment(zaf::AxisAlignment::Center);

    auto control = zaf::Create<zaf::Control>();
    control->SetRect(zaf::Rect(10, 10, 500, 100));
    control->SetLayouter(layouter);
    control->SetBorder(1);
    control->SetBorderColor(zaf::Color::Black);

    for (int count = 0; count < 3; ++count) {

        auto child = zaf::Create<zaf::Control>();
        child->SetBorder(1);
        child->SetBorderColor(zaf::Color::Green);
        child->SetMaximumWidth((count + 2) * 10);
        control->AddChild(child);
    }

    root_control->AddChild(control);

    application.SetMainWindow(window);
}