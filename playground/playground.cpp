#include <Windows.h>
#include <WindowsX.h>
#include <richedit.h>
#include <richole.h>
#include <atlbase.h>
#include <atlctl.h>
#include <fstream>
#include <charconv>
#include <zaf/application.h>
#include <zaf/base/log.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/container/utility/range.h>
#include <zaf/base/stream.h>
#include <zaf/base/string/to_string.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include <zaf/window/dialog.h>
#include <zaf/control/button.h>
#include <zaf/control/rich_edit.h>
#include <zaf/window/message/hit_test_result.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/image/image.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/dpi.h>
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
#include <zaf/control/linear_box.h>
#include <zaf/base/registry/registry.h>
#include <zaf/base/error/error.h>
#include <zaf/object/type_definition.h>
#include <zaf/resource/resource_factory.h>
#include <zaf/object/boxing/string.h>
#include <zaf/object/boxing/boxing.h>
#include <zaf/control/combo_box.h>
#include <zaf/control/tree_control.h>
#include <zaf/control/tree_data_source.h>
#include <zaf/control/tree_control_delegate.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scroll_bar_thumb.h>
#include <zaf/control/check_box.h>
#include <zaf/rx/creation.h>
#include <zaf/rx/cancel.h>
#include <zaf/rx/scheduler.h>
#include <zaf/rx/timer.h>
#include <zaf/object/internal/property_helper.h>
#include <zaf/control/property_grid.h>
#include <zaf/window/popup_menu.h>
#include <zaf/control/menu_separator.h>
#include <zaf/control/rich_edit/embedded_object.h>

class MyOLEObject : public zaf::rich_edit::EmbeddedObject {
public:
    MyOLEObject() {
        this->SetSize(zaf::Size{ 100, 30 });
    }

    GUID ClassID() const override {
        return { 0xe16f8acd, 0x5b3a, 0x4167, { 0xa4, 0x49, 0xdc, 0x57, 0xd, 0xd4, 0x44, 0x59 } };
    }

    void Paint(
        zaf::Canvas& canvas,
        const zaf::Rect& dirty_rect, 
        const zaf::rich_edit::PaintContext& context) override {

        auto color = context.IsInSelectionRange() ? zaf::Color::Blue() : zaf::Color::Green();

        canvas.DrawRectangle(
            zaf::Rect{ zaf::Point{}, this->Size() },
            canvas.Renderer().CreateSolidColorBrush(color));
    }

    void OnMouseDown(const zaf::rich_edit::MouseDownContext& context) override {

        context.EventInfo().MarkAsHandled();
    }

    void OnMouseUp(const zaf::rich_edit::MouseUpContext& context) override {

        context.EventInfo().MarkAsHandled();
    }

    void OnMouseCursorChanging(
        const zaf::rich_edit::MouseCursorChangingContext& context) override {
        SetCursor(LoadCursor(nullptr, IDC_HAND));
        context.EventInfo().MarkAsHandled();
    }
};

void BeginRun(const zaf::ApplicationBeginRunInfo& event_info);

class Window : public zaf::Window {
protected:
    void AfterParse() override {

        __super::AfterParse();

        this->RootControl()->SetBackgroundColor(zaf::Color::White());
        this->RootControl()->SetLayouter(zaf::Create<zaf::VerticalLayouter>());

        rich_edit_ = zaf::Create<zaf::RichEdit>();
        rich_edit_->SetMargin(zaf::Frame{ 10, 10, 10, 10 });
        rich_edit_->SetBorder(zaf::Frame{ 10, 10, 10, 10 });
        rich_edit_->SetFontSize(22);
        rich_edit_->SetIsMultiline(true);

        auto container = zaf::Create<zaf::ScrollableControl>();
        container->SetScrollContent(rich_edit_);
        container->SetPadding(zaf::Frame{ 10, 10, 10, 10 });
        container->SetBackgroundColor(zaf::Color::Yellow());

        this->RootControl()->AddChild(container);

        InitializeOLEObject();
    }

private:
    void InitializeOLEObject() {

        zaf::COMObject<MyOLEObject> object{ new MyOLEObject };

        rich_edit_->InsertObject(object);
    }

private:
    std::shared_ptr<zaf::RichEdit> rich_edit_;
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
    window->SetIsSizable(true);
    window->SetHasTitleBar(true);
    window->SetContentSize(zaf::Size{ 600, 400 });
    window->Show();

    zaf::Application::Instance().SetMainWindow(window);
}
