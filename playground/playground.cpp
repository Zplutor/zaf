#include <Windows.h>
#include <zaf/application.h>
#include <zaf/base/log.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/container/utility/range.h>
#include <zaf/base/stream.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include <zaf/window/dialog.h>
#include <zaf/control/button.h>
#include <zaf/control/text_box.h>
#include <zaf/window/message/hit_test_result.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/image/image.h>
#include <zaf/graphic/font/font.h>
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
#include <zaf/resource/resource_manager.h>
#include <zaf/parsing/parsers/control_parser.h>
#include <zaf/object/string.h>
#include <zaf/object/boxing.h>
#include <zaf/control/combo_box.h>
#include <zaf/control/tree_control.h>
#include <zaf/control/tree_data_source.h>
#include <zaf/control/tree_control_delegate.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/check_box.h>
#include <zaf/rx/creation.h>
#include <zaf/rx/cancel.h>
#include <zaf/rx/scheduler.h>
#include <zaf/rx/timer.h>

void BeginRun(const zaf::ApplicationBeginRunInfo& event_info);


int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
) {

    auto& application = zaf::Application::Instance();
    application.Subscriptions() += application.BeginRunEvent().Subscribe(BeginRun);

    application.Initialize({});
    application.Run();
}


zaf::Subscription g_s;


void BeginRun(const zaf::ApplicationBeginRunInfo& event_info) {

    auto text_box = zaf::Create<zaf::TextBox>();
    text_box->SetText(L"TextBox");
    text_box->SetSize(zaf::Size{ 500, 300 });
    zaf::Application::Instance().Subscriptions() += text_box->SelectionChangeEvent().Subscribe([](const zaf::TextBoxSelectionChangeInfo& info) {
    
        auto selection_range = info.text_box->GetSelectionRange();
    });

    auto window = zaf::Create<zaf::Window>();

    window->SetClientSize(zaf::Size{ 400, 300 });
    window->GetRootControl()->AddChild(text_box);
    window->Show();

    zaf::Application::Instance().SetMainWindow(window);

    auto observable = zaf::rx::Create<std::wstring>(
        zaf::Scheduler::CreateOnSingleThread(), 
        [](zaf::Observer<std::wstring>& observer, zaf::CancelToken& cancel_token) {
    
        std::this_thread::sleep_for(std::chrono::seconds(2));

        if (!cancel_token.IsCancelled()) {
            ZAF_LOG() << L"Not cancelled";
            observer.OnNext(L"Result");
        }
        else {
            ZAF_LOG() << L"Cancelled";
        }
    });

    g_s = observable.Subscribe([](const std::wstring& r) {
        ZAF_LOG() << L"Result: " << r;
    });

    zaf::Application::Instance().Subscriptions() += zaf::rx::Timer(std::chrono::seconds(1), zaf::Scheduler::Main()).Subscribe([](int) {
        g_s.Unsubscribe();
    });
}