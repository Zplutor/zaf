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

void BeginRun(const zaf::ApplicationBeginRunInfo& event_info);

class Window : public zaf::Window {
protected:
    void AfterParse() override {

        __super::AfterParse();

        this->RootControl()->SetBackgroundColor(zaf::Color::White());
        this->RootControl()->SetLayouter(zaf::Create<zaf::VerticalLayouter>());

        auto horizontal_box = zaf::Create<zaf::HorizontalBox>();
        horizontal_box->AddChild(InitializeTextBox());
        horizontal_box->AddChild(InitializeRichEdit());
        RootControl()->AddChild(horizontal_box);

        Subscriptions() += rich_edit_->TextChangedEvent().Subscribe(
            [this](const zaf::TextChangedInfo& event_info) {
        
            text_box_->SetText(rich_edit_->Text());
        });

        //rich_edit_->SetText(GenerateInitialText());
        InitializeButton();
    }

private:
    std::shared_ptr<zaf::Control> InitializeTextBox() {

        text_box_ = zaf::Create<zaf::TextBox>();
        text_box_->SetFontSize(20);
        text_box_->SetTextAlignment(zaf::TextAlignment::Center);
        text_box_->SetParagraphAlignment(zaf::ParagraphAlignment::Center);

        auto scroll_control = zaf::Create<zaf::ScrollableControl>();
        scroll_control->SetScrollContent(text_box_);
        scroll_control->SetAutoHideScrollBars(true);
        scroll_control->VerticalScrollBar()->SetLargeChange(20);
        return scroll_control;
    }

    std::shared_ptr<zaf::Control> InitializeRichEdit() {

        rich_edit_ = zaf::Create<zaf::RichEdit>();
        rich_edit_->SetIsMultiline(true);
        rich_edit_->SetFontSize(20);
        rich_edit_->SetBorder({});

        auto scroll_control = zaf::Create<zaf::ScrollableControl>();
        scroll_control->SetScrollContent(rich_edit_);
        scroll_control->SetAutoHideScrollBars(true);
        return scroll_control;
    }

    std::wstring GenerateInitialText() {

        std::wstring content;
        for (int line = 0; line < 30; ++line) {

            std::wstring padding(20, L'=');
            content += padding + L' ' + std::to_wstring(line) + L' ' + padding + L"\r\n";
        }
        return content;
    }

    void InitializeButton() {

        button_ = zaf::Create<zaf::Button>();
        button_->SetFixedHeight(30);
        Subscriptions() += button_->ClickEvent().Subscribe(std::bind([this]() {

            text_box_->SetSelectionRange(zaf::Range{ 2, 5 });
        }));

        RootControl()->AddChild(button_);
    }

private:
    std::shared_ptr<zaf::TextBox> text_box_;
    std::shared_ptr<zaf::RichEdit> rich_edit_;
    std::shared_ptr<zaf::Button> button_;
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
    window->SetSize(zaf::Size{ 1200, 600 });
    window->SetIsSizable(true);
    window->SetHasTitleBar(true);

    window->Show();

    zaf::Application::Instance().SetMainWindow(window);
}
