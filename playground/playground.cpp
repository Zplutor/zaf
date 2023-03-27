#include <Windows.h>
#include <WindowsX.h>
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
#include <zaf/control/text_box.h>
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

void BeginRun(const zaf::ApplicationBeginRunInfo& event_info);


void AddSubMenuItemsToMenuItem(zaf::MenuItem& menu_item, int deep) {

    if (deep >= 2) {
        return;
    }

    for (int count = 0; count < 3; ++count) {

        auto sub_menu_item = zaf::Create<zaf::MenuItem>();
        sub_menu_item->SetText(L"Sub menu item " + std::to_wstring(count));

        menu_item.AddSubMenuItem(sub_menu_item);

        AddSubMenuItemsToMenuItem(*sub_menu_item, deep + 1);
    }
}


class Window : public zaf::Window {
protected:
    void AfterParse() override {

        __super::AfterParse();

        this->RootControl()->SetBackgroundColor(zaf::Color::White());
        this->RootControl()->SetLayouter(zaf::Create<zaf::VerticalLayouter>());

        text_box_ = zaf::Create<zaf::TextBox>();
        text_box_->SetFontSize(22);

        button_ = zaf::Create<zaf::Button>();
        button_->SetFixedHeight(30);

        this->RootControl()->AddChildren({ text_box_, button_ });
    }

    void OnMessageHandled(const zaf::MessageHandledInfo& event_info) override {

        __super::OnMessageHandled(event_info);

        if (event_info.Message().ID() == WM_RBUTTONUP) {

            auto context_menu = zaf::Create<zaf::PopupMenu>();
            context_menu->SetOwner(shared_from_this());

            for (int count = 0; count < 6; ++count) {

                auto menu_item = zaf::Create<zaf::MenuItem>();
                menu_item->SetText(L"Menu item " + std::to_wstring(count));

                Subscriptions() += menu_item->MouseUpEvent().Subscribe([](const zaf::MouseUpInfo& event_info) {
                    event_info.IsHandled();
                });

                if (count == 2) {
                    context_menu->AddMenuItem(zaf::Create<zaf::MenuSeparator>());
                }

                if (count > 2) {
                    AddSubMenuItemsToMenuItem(*menu_item, 0);
                }

                if (count == 5) {
                    menu_item->SetIsEnabled(false);
                }

                context_menu->AddMenuItem(menu_item);
            }

            auto mouse_position = zaf::MouseMessage(event_info.Message()).MousePosition();
            context_menu->Popup(ToScreenPosition(mouse_position));
        }
    }

private:
    std::shared_ptr<zaf::TextBox> text_box_;
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
    window->SetIsSizable(true);
    window->SetHasTitleBar(true);
    window->SetContentSize(zaf::Size{ 600, 400 });
    window->Show();

    zaf::Application::Instance().SetMainWindow(window);
}
