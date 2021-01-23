#include <Windows.h>
#include <zaf/application.h>
#include <zaf/base/log.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/container/utility/range.h>
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
#include <zaf/control/tree_control.h>
#include <zaf/control/tree_data_source.h>
#include <zaf/control/tree_control_delegate.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/rx/scheduler.h>
#include <zaf/rx/creation.h>

void BeginRun(const zaf::ApplicationBeginRunInfo& event_info);

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

    auto& application = zaf::Application::Instance();
    application.Subscriptions() += application.BeginRunEvent().Subscribe(BeginRun);

    application.Initialize({});

    ZAF_LOG() << std::this_thread::get_id() << L"Main thread.";

    zaf::rx::Create<std::string>(
        [](zaf::Observer<std::string> observer) {

        ZAF_LOG() << std::this_thread::get_id() << ' ' << L"Subscribe";

        observer.OnNext("a");
        observer.OnNext("b");
        observer.OnNext("c");
        return zaf::Subscription{};
    })
    .SubscribeOn(zaf::Scheduler::CreateOnSingleThread())
    .Subscribe([](const std::string& string) {
    
        ZAF_LOG() << std::this_thread::get_id() << ' ' << L"OnNext";
    });

    application.Run();
}


void BeginRun(const zaf::ApplicationBeginRunInfo& event_info) {

    auto window = zaf::Create<zaf::Window>();

    auto root_control = zaf::Create<RootControl>();
    root_control->SetName(L"Root control");
    window->SetIsPopup(true);
    window->SetHasBorder(true);
    window->SetHasTitleBar(true);
    window->SetIsSizable(true);
    window->SetRootControl(root_control);
    window->Show();

    zaf::Application::Instance().SetMainWindow(window);
}