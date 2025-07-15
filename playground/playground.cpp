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
#include <zaf/io/stream/stream.h>
#include <zaf/base/string/to_string.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include <zaf/window/dialog.h>
#include <zaf/control/button.h>
#include <zaf/control/rich_edit.h>
#include <zaf/window/message/hit_test_result.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/image.h>
#include <zaf/graphic/font.h>
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
#include <zaf/rx/scheduler.h>
#include <zaf/rx/timer.h>
#include <zaf/control/property_grid.h>
#include <zaf/window/popup_menu.h>
#include <zaf/control/menu_separator.h>
#include <zaf/control/rich_edit/embedded_object.h>
#include <zaf/control/rich_edit/ole_callback.h>
#include <zaf/control/text_box.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/input/mouse.h>
#include <zaf/control/control_object.h>
#include <zaf/control/textual/active_inline_object.h>
#include <zaf/control/list_data_source.h>

void BeginRun(const zaf::BeginRunInfo& event_info);

class DataSource : public zaf::ListDataSource {
public:
    DataSource() {

        for (auto index : zaf::Range(0, 10)) {
            data_.push_back(zaf::Box(std::to_wstring(index)));
        }
    }

    std::size_t GetDataCount() const override {
        return data_.size();
    }

    std::shared_ptr<zaf::Object> GetDataAtIndex(std::size_t index) const override {
        return data_[index];
    }

    void Add() {

        auto new_data = zaf::Box(std::to_wstring(100));
        data_.insert(data_.begin(), new_data);
        NotifyDataAdded(0, 1);
    }

    void Move() {

        auto data = data_[0];
        data_.erase(data_.begin());
        data_.insert(std::next(data_.begin(), 2), data);

        NotifyDataMoved(0, 2);
    }

private:
    std::vector<std::shared_ptr<zaf::WideString>> data_;
};


class Delegate : public zaf::ListControlDelegate {
public:
    float EstimateItemHeight(
        std::size_t item_index,
        const std::shared_ptr<zaf::Object>& item_data) override {

        return 40;
    }

    std::shared_ptr<zaf::ListItem> CreateItem(
        std::size_t item_index,
        const std::shared_ptr<zaf::Object>& item_data) {

        auto result = __super::CreateItem(item_index, item_data);

        auto old_picker = result->BackgroundColorPicker();
        result->SetBackgroundColorPicker(zaf::ColorPicker([old_picker](const zaf::Control& control) {
        
            if (control.IsFocused()) {
                return zaf::Color::Green();
            }

            if (control.IsMouseOver()) {
                return zaf::Color::Yellow();
            }

            return old_picker(control);
        }));

        return result;
    }
};


class Window : public zaf::Window {
protected:
    void Initialize() override {

        __super::Initialize();

        this->RootControl()->SetLayouter(zaf::Create<zaf::VerticalLayouter>());

        list_ = zaf::Create<zaf::ListControl>();
        list_->SetDataSource(zaf::Create<DataSource>());
        list_->SetSelectionMode(zaf::ListSelectionMode::ExtendedMultiple);

        Subscriptions() += list_->ContextMenuEvent().Subscribe(
            [](const zaf::ListControlContextMenuInfo& event_info) {
        
                auto menu_item1 = zaf::Create<zaf::MenuItem>();
                menu_item1->SetText(L"Menu1");

                auto menu_item2 = zaf::Create<zaf::MenuItem>();
                menu_item2->SetText(L"Menu2");

                auto menu = zaf::Create<zaf::PopupMenu>();
                menu->AddMenuItem(menu_item1);
                menu->AddMenuItem(menu_item2);

                event_info.SetMenu(menu);
            });

        this->RootControl()->AddChild(list_);

        auto button = zaf::Create<zaf::Button>();
        button->SetFixedHeight(30);
        button->SetText(L"Add");
        Subscriptions() += button->ClickEvent().Subscribe(std::bind([this]() {
            //data_source_->Add();
        }));
        this->RootControl()->AddChild(button);
    }

private:
    std::shared_ptr<zaf::ListControl> list_;
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