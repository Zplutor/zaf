#include <Windows.h>
#include <WindowsX.h>
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
#include <zaf/object/object_type.h>
#include <zaf/object/internal/reflection_manager.h>
#include <zaf/object/object_property.h>
#include <zaf/object/parsing/helpers.h>
#include <zaf/object/parsing/xaml_reader.h>
#include <zaf/object/creation.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/label.h>
#include <zaf/control/image_box.h>
#include <zaf/base/registry/registry.h>
#include <zaf/base/error/error.h>
#include <zaf/object/type_definition.h>
#include <zaf/resource/resource_manager.h>
#include <zaf/object/boxing/string.h>
#include <zaf/object/boxing/boxing.h>
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
#include <zaf/object/internal/property_helper.h>

void BeginRun(const zaf::ApplicationBeginRunInfo& event_info);


class Window : public zaf::Window {
public:
    void AfterParse() override {

        __super::AfterParse();

        auto label = zaf::Create<zaf::Label>();
        label->SetRect(zaf::Rect{ 20, 20, 100, 30 });
        label->SetText(L"Label");
        label->SetTooltip(L"This is a label");
        Subscriptions() += label->MouseHoverEvent().Subscribe(
            [this](const zaf::ControlMouseHoverInfo& event_info) {
        
            OutputDebugString(L"Label hover");
        });
        this->RootControl()->AddChild(label);
        this->RootControl()->SetBackgroundColor(zaf::Color::White());
        this->RootControl()->SetBorder(zaf::Frame{ 1 });
        this->RootControl()->SetBorderColor(zaf::Color::Black());

        auto button = zaf::Create<zaf::Button>();
        button->SetRect(zaf::Rect{ 20, 60, 100, 30 });
        button->SetText(L"Button");
        button->SetTooltip(L"This is a button");
        Subscriptions() += button->MouseHoverEvent().Subscribe(
            [this](const zaf::ControlMouseHoverInfo& event_info) {
        
            OutputDebugString(L"Button hover");
        });
        this->RootControl()->AddChild(button);
    }

protected:
    bool ReceiveMessage(const zaf::Message& message, LRESULT& result) override {
        return __super::ReceiveMessage(message, result);
    }
};


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


void BeginRun(const zaf::ApplicationBeginRunInfo& event_info) {

    auto window = zaf::Create<Window>();
    window->SetSize(zaf::Size{ 300, 300 });
    window->Show();

    zaf::Application::Instance().SetMainWindow(window);
}
