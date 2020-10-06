#include <Windows.h>
#include <zaf/application.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include <zaf/window/dialog.h>
#include <zaf/control/button.h>
#include <zaf/control/text_box.h>
#include <zaf/window/message/hit_test_result.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/image/image.h>
#include <zaf/control/list_box.h>
#include <zaf/reflection/reflection_type.h>
#include <zaf/reflection/reflection_manager.h>
#include <zaf/parsing/helpers.h>
#include <zaf/parsing/xaml_reader.h>
#include <zaf/reflection/creation.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/label.h>
#include <zaf/control/image_box.h>
#include <zaf/base/registry/registry.h>
#include <zaf/base/error/error.h>
#include <zaf/reflection/reflection_type_definition.h>
#include <zaf/parsing/parsers/control_parser.h>
#include <zaf/object/string.h>
#include <zaf/object/boxing.h>
#include <zaf/control/combo_box.h>

void BeginRun(zaf::Application&);

class RootControl : public zaf::Control {
public:
    ZAF_DECLARE_REFLECTION_TYPE
};

ZAF_DEFINE_REFLECTION_TYPE(RootControl)
ZAF_DEFINE_END


int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
) {

    auto& application = zaf::Application::GetInstance();
    application.GetBeginRunEvent().AddListener(BeginRun);

    application.Initialize({});
    application.Run();
}


void BeginRun(zaf::Application& application) {

    auto window = zaf::Create<zaf::Window>();

    auto root_control = zaf::Create<RootControl>();
    window->SetBorderStyle(zaf::Window::BorderStyle::Normal);
    window->SetCanMaximize(true);
    window->SetIsSizable(true);
    window->SetRootControl(root_control);
    window->Show();

    auto combo_box = zaf::Create<zaf::ComboBox>();
    combo_box->SetRect(zaf::Rect(10, 10, 100, 30));
    
    /*
    auto list_box = combo_box->GetDropDownListBox();
    list_box->AddItem(zaf::Box(L"1"));
    list_box->AddItem(zaf::Box(L"2"));
    list_box->AddItem(zaf::Box(L"3"));
    list_box->AddItem(zaf::Box(L"4"));
    list_box->AddItem(zaf::Box(std::vector<std::string>{ "0", "1" }));
    */

    auto list_box = zaf::Create<zaf::ListBox>();
    list_box->SetRect(zaf::Rect(10, 10, 200, 400));
    list_box->AddItem(zaf::Box(std::vector<std::string>{ "0", "1" }));
    list_box->AddItem(zaf::Box(L"1"));
    list_box->AddItem(zaf::Box(L"2"));
    list_box->AddItem(zaf::Box(L"3"));
    list_box->AddItem(zaf::Box(L"4"));
    list_box->AddItem(zaf::Box(L"5"));
    
    auto item = zaf::Create<zaf::Button>();
    item->SetText(L"button");
    item->SetHeight(35);
    list_box->AddItem(item);
    
    //root_control->SetLayouter(zaf::Create<zaf::HorizontalLayouter>());
    root_control->AddChild(list_box);

    application.SetMainWindow(window);
}