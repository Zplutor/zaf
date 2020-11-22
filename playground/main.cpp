#include <Windows.h>
#include <zaf/application.h>
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

void BeginRun(const zaf::ApplicationBeginRunInfo& event_info);

class RootControl : public zaf::Control {
public:
    ZAF_DECLARE_REFLECTION_TYPE
};

ZAF_DEFINE_REFLECTION_TYPE(RootControl)
ZAF_DEFINE_END

bool g_append_text{};

class TreeItemSource : public zaf::TreeDataSource, public zaf::TreeControlDelegate {
public:
    TreeItemSource() {

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

        /*
        std::function<std::vector<std::shared_ptr<Node>>(int count, int deep)> create_nodes;
        create_nodes = [&create_nodes](int count, int deep) {

            std::vector<std::shared_ptr<Node>> nodes;
            for (auto _ : zaf::Range(count)) {

                auto node = zaf::Create<Node>();
                if (deep < 3) {
                    node->children = create_nodes(3, deep + 1);
                }
                nodes.push_back(node);
            }
            return nodes;
        };

        nodes_ = create_nodes(5, 0);
        */
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

        if (parent_item_data == nullptr) {

            if (g_append_text) {
                text.append(L" UP!");
            }
        }

        return text;
    }

    void UpdateFirstLevel() {

        NotifyDataUpdate(nullptr, 0, 4);
    }

    void AddData() {

        auto& node = nodes_[0];

        std::size_t old_size = node->children.size();

        for (auto _ : zaf::Range(4)) {

            auto child = std::make_shared<Node>();
            node->children.insert(node->children.begin(), child);
        }

        NotifyDataAdd(node, 0, node->children.size() - old_size);
        NotifyDataUpdate(nullptr, 0, 1);
    }

private:
    class Node : public zaf::Object {
    public:
        Node() {
            static int seed = 0;
            value = ++seed;
        }

        std::wstring ToString() const override {
            return std::to_wstring(value);
        }

        int value{};
        std::vector<std::shared_ptr<Node>> children;
    };

private:
    std::vector<std::shared_ptr<Node>> nodes_;
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
    window->SetBorderStyle(zaf::Window::BorderStyle::Normal);
    window->SetCanMaximize(true);
    window->SetIsSizable(true);
    window->SetRootControl(root_control);
    window->Show();

    auto item_source = zaf::Create<TreeItemSource>();

    auto tree_control = zaf::Create<zaf::TreeControl>();
    tree_control->SetRect(zaf::Rect{ 10, 10, 300, 400 });
    tree_control->SetDataSource(item_source);
    tree_control->SetDelegate(item_source);
    tree_control->SetSelectionMode(zaf::SelectionMode::ExtendedMultiple);

    tree_control->Subscriptions() += tree_control->SelectionChangeEvent().Subscribe(
        [](const zaf::TreeControlSelectionChangeInfo& event_info) {
    
            for (const auto& each_data : event_info.tree_control->GetAllSelectedItemData()) {
                OutputDebugString((each_data->ToString() + L"\r\n").c_str());
            }
        }
    );

    root_control->AddChild(tree_control);

    auto button = zaf::Create<zaf::Button>();
    button->SetText(L"Add");
    button->SetRect(zaf::Rect{ 310, 10, 100, 30 });
    button->Subscriptions() += button->ClickEvent().Subscribe(std::bind([item_source]() {
        
        //g_append_text = !g_append_text;
        item_source->AddData();
    }));

    root_control->AddChild(button);

    zaf::Application::Instance().SetMainWindow(window);
}