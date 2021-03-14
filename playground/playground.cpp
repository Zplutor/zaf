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

void BeginRun(const zaf::ApplicationBeginRunInfo& event_info);

class RootControl : public zaf::Control {
public:
    ZAF_DECLARE_REFLECTION_TYPE
};

class MyControl : public zaf::Control {
protected:
    void Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) override {

        __super::Paint(canvas, dirty_rect);

        
        canvas.SetBrushWithColor(zaf::Color::Red());
        canvas.DrawLine(zaf::Point{ 0, 0 }, zaf::Point{ 0, 100 }, 1);
        
        /*
        zaf::RoundedRect bubble_rect(zaf::Rect(zaf::Point(), GetSize()), 4, 4);

        canvas.SetBrushWithColor(zaf::Color::Blue());
        canvas.DrawRoundedRectangleFrame(bubble_rect, 1);
        */
    }
};

ZAF_DEFINE_REFLECTION_TYPE(RootControl)
ZAF_DEFINE_END

class TreeItemSource : public zaf::TreeDataSource, public zaf::TreeControlDelegate {
public:
    TreeItemSource() {

        /*
        for (auto _ : zaf::Range(3)) {

            auto node = zaf::Create<Node>();

            if (_ == 1) {

                for (auto _ : zaf::Range(3)) {

                    auto child_node = zaf::Create<Node>();
                    node->children.push_back(child_node);
                }
            }

            nodes_.push_back(node);
        }
        */

        std::function<std::vector<std::shared_ptr<Node>>(int count, int deep)> create_nodes;
        create_nodes = [&create_nodes](int count, int deep) {

            std::vector<std::shared_ptr<Node>> nodes;
            for (auto _ : zaf::Range(count)) {

                auto node = zaf::Create<Node>();
                if (deep < 3) {
                    node->children = create_nodes(2, deep + 1);
                }
                nodes.push_back(node);
            }
            return nodes;
        };

        nodes_ = create_nodes(2, 0);
        nodes_.push_back(zaf::Create<Node>());
    }


    bool DoesDataHasChildren(const std::shared_ptr<zaf::Object>& parent_data) override {

        auto child_count = GetChildDataCount(parent_data);
        return child_count > 0;
    }


    std::size_t GetChildDataCount(const std::shared_ptr<zaf::Object>& parent_data) override {

        if (!parent_data) {
            return nodes_.size();
        }

        auto value = zaf::TryUnbox<Node>(parent_data);
        if (!value) {
            return 0;
        }

        return value->children.size();
    }


    std::shared_ptr<zaf::Object> GetChildDataAtIndex(
        const std::shared_ptr<zaf::Object>& parent_data,
        std::size_t index) override {

        if (!parent_data) {
            return nodes_[index];
        }

        auto value = zaf::TryUnbox<Node>(parent_data);
        if (!value) {
            return 0;
        }

        return value->children[index];
    }


    float EstimateItemHeight(
        const std::shared_ptr<zaf::Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<zaf::Object>& item_data) override {

        return 30;
    }


    std::wstring GetItemText(
        const std::shared_ptr<zaf::Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<zaf::Object>& item_data) override {

        std::wstring text = item_data->ToString();
        return text;
    }

    bool CanExpandItem(
        const std::shared_ptr<zaf::Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<zaf::Object>& item_data) override {

        auto node = dynamic_cast<Node*>(item_data.get());
        if (!node) {
            return false;
        }

        return (node->value % 2) == 0;
    }

    bool CanCollapseItem(
        const std::shared_ptr<zaf::Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<zaf::Object>& item_data) override {

        auto node = dynamic_cast<Node*>(item_data.get());
        if (!node) {
            return false;
        }

        return node->value == 24;
    }

    void UpdateSecondLevel() {

        for (const auto& each_node : nodes_) {

            for (const auto& each_child : each_node->children) {
                each_child->append_text = !each_child->append_text;
            }

            NotifyDataUpdate(each_node, 0, each_node->children.size());
        }
    }

    void AddDataToSecondLevel() {

        auto& node = nodes_[2];

        std::size_t old_size = node->children.size();

        for (auto _ : zaf::Range(3)) {

            auto child = std::make_shared<Node>();
            node->children.insert(node->children.end(), child);
        }

        NotifyDataAdd(node, old_size, node->children.size() - old_size);
        //NotifyDataUpdate(nullptr, 0, 1);
    }

    void RemoveDataInSecondLevel() {

        auto& node = nodes_[2];

        auto old_size = node->children.size();

        for (auto _ : zaf::Range(3)) {
            if (!node->children.empty()) {
                node->children.erase(node->children.begin());
            }
        }

        auto remove_count = old_size - node->children.size();
        NotifyDataRemove(node, 0, remove_count);
    }

private:
    class Node : public zaf::Object {
    public:
        Node() {
            static int seed = 0;
            value = ++seed;
        }

        std::wstring ToString() const override {
            auto result = std::to_wstring(value);
            if (append_text) {
                result += L"!UP!";
            }
            return result;
        }

        int value{};
        std::vector<std::shared_ptr<Node>> children;
        bool append_text{};
    };

private:
    std::vector<std::shared_ptr<Node>> nodes_;
};


class MyLabel : public zaf::Label {
public:

protected:
    zaf::Rect GetTextRect() override {

        zaf::Rect rect{ zaf::Rect{ 0, 0, 200, 200 } };
        return rect;
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

    auto window = zaf::Create<zaf::Window>();

    auto root_control = zaf::Create<RootControl>();
    root_control->SetName(L"Root control");
    window->SetIsPopup(true);
    window->SetHasBorder(true);
    window->SetHasTitleBar(true);
    window->SetIsSizable(true);
    window->SetRootControl(root_control);
    window->Show();

    auto my_control = zaf::Create<MyControl>();
    my_control->SetRect(zaf::Rect{ 0, 0.5, 100, 17.7207 });
    root_control->AddChild(my_control);

    zaf::GetResourceManager().LoadUri(L"res:///main_window.xml");

    zaf::Application::Instance().SetMainWindow(window);
}