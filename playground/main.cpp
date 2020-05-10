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
#include <zaf/graphic/image/image.h>
#include <zaf/control/combo_box.h>
#include <zaf/reflection/reflection_type.h>
#include <zaf/reflection/reflection_manager.h>
#include <zaf/parsing/helpers.h>
#include <zaf/parsing/xaml_reader.h>
#include <zaf/reflection/creation.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/label.h>
#include <zaf/control/image_box.h>

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

    auto image = zaf::CreateObjectFromXaml<zaf::Image>(L"<BitmapImage Uri=\"file:///C:\\Users\\zplutor\\Desktop\\background.png\" />");

    /*
    auto image = zaf::Image::FromFile(LR"(C:\Users\zplutor\Desktop\²¶»ñ.PNG)");
    */

    auto image_control = zaf::Create<zaf::Control>();
    image_control->SetRect(zaf::Rect{ 100, 100, 200, 200 });
    image_control->SetBorder(5);
    image_control->SetBorderColor(zaf::Color{ 0, 0, 0, 0.5 });
    image_control->SetBackgroundImage(image);
    image_control->SetBackgroundImageLayout(zaf::ImageLayout::Tile);

    root_control->AddChild(image_control);

    auto image_box = zaf::Create<zaf::ImageBox>();
    image_box->SetRect(zaf::Rect{ 330, 50, 200, 200 });
    image_box->SetUri(L"file:///C:\\Users\\zplutor\\Desktop\\background.png");
    image_box->SetPadding(5);
    image_box->SetBorder(5);
    image_box->SetBackgroundColor(zaf::Color::Green);
    image_box->SetBorderColor(zaf::Color::Black);
    image_box->ResizeToPreferredSize();
    root_control->AddChild(image_box);

    std::filesystem::path path{ L"\\haha "};
    bool v = path.is_absolute();

    application.SetMainWindow(window);
}