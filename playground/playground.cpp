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
#include <zaf/control/rich_edit/ole_callback.h>
#include <zaf/control/text_box.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/graphic/stroke_properties.h>
#include <zaf/control/textual/dynamic_inline_object.h>
#include <zaf/input/mouse.h>

void BeginRun(const zaf::BeginRunInfo& event_info);

class InlineObject : public zaf::textual::DynamicInlineObject {
public:
    ZAF_DECLARE_TYPE;

    zaf::TextInlineObjectMetrics GetMetrics() const override {

        zaf::TextInlineObjectMetrics result;
        result.SetWidth(100);
        result.SetHeight(30);
        result.SetHeightAboveBaseline(24);
        return result;
    }

protected:
    void Paint(zaf::Canvas& canvas) const override {

        canvas.DrawRectangle(
            zaf::Rect{ {}, this->Size() }, 
            IsInSelectionRange() ? zaf::Color::Blue() :
            IsMouseOver() ? zaf::Color::Green() : zaf::Color::Red());
    }

    bool HitTest(bool is_mouse_inside) override {
        return is_mouse_inside;
    }

    void OnMouseCursorChanging(const zaf::textual::MouseCursorChangingInfo& event_info) override {

        zaf::Mouse::SetCursor(zaf::Cursor::Normal());
        event_info.MarkAsHandled();
    }

    void OnMouseEnter(const zaf::textual::MouseEnterInfo& event_info) override {

        __super::OnMouseEnter(event_info);

        NeedRepaint();
    }

    void OnMouseLeave(const zaf::textual::MouseLeaveInfo& event_info) override {

        __super::OnMouseLeave(event_info);

        NeedRepaint();
    }

    void OnMouseDown(const zaf::textual::MouseDownInfo& event_info) override {

        __super::OnMouseDown(event_info);

        Host()->SetSelectionRange(*this->RangeInHost());

        event_info.MarkAsHandled();
    }
};


ZAF_DEFINE_TYPE(InlineObject);
ZAF_DEFINE_TYPE_END;


class Window : public zaf::Window {
protected:
    void AfterParse() override {

        __super::AfterParse();

        this->RootControl()->SetBackgroundColor(zaf::Color::White());
        this->RootControl()->SetLayouter(zaf::Create<zaf::VerticalLayouter>());

        auto text_box = zaf::Create<zaf::TextBox>();
        text_box->SetFontSize(20);
        text_box->SetIsEditable(true);
        text_box->SetText(
L"this->RootControl()->SetLayouter(zaf::Create<zaf::VerticalLayouter>());\r\n"
L"this->RootControl()->SetLayouter(zaf::Create<zaf::VerticalLayouter>());\r\n"
L"this->RootControl()->SetLayouter(zaf::Create<zaf::VerticalLayouter>());\r\n"
L"this->RootControl()->SetLayouter(zaf::Create<zaf::VerticalLayouter>());\r\n"
L"this->RootControl()->SetLayouter(zaf::Create<zaf::VerticalLayouter>());\r\n"
L"this->RootControl()->SetLayouter(zaf::Create<zaf::VerticalLayouter>());\r\n");
        text_box->SetTextColorInRange(zaf::Color::Red(), zaf::Range{ 5, 6 });
        text_box->SetTextBackColorInRange(zaf::Color::Yellow(), zaf::Range{ 30, 10 });

        text_box->AttachInlineObjectToRange(std::make_shared<InlineObject>(), zaf::Range{ 10, 5 });

        text_box->SetLineSpacing(zaf::LineSpacing{ zaf::LineSpacingMethod::Uniform, 50, 40 });
        text_box->SetTextBackPadding(zaf::Frame{ 0, 20, 0, 0 });

        this->RootControl()->AddChild(text_box);

        auto button = zaf::Create<zaf::Button>();
        button->SetFixedHeight(30);
        Subscriptions() += button->ClickEvent().Subscribe(std::bind([this, text_box]() {
            text_box->SetText(L"Text changed.");
        }));

        this->RootControl()->AddChild(button);
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


void BeginRun(const zaf::BeginRunInfo& event_info) {

    auto window = zaf::Create<Window>();
    window->SetSize(zaf::Size{ 1200, 600 });
    window->SetIsSizable(true);
    window->SetHasTitleBar(true);

    window->Show();

    zaf::Application::Instance().SetMainWindow(window);
}
