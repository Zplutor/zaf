#include <zaf/window/inspector/inspector_window.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/control/button.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/tree_control.h>
#include <zaf/creation.h>
#include <zaf/window/inspector/internal/inspect_control_item.h>
#include <zaf/window/inspector/internal/inspect_data_source.h>
#include <zaf/window/inspector/internal/inspect_item.h>

namespace zaf {
namespace {

constexpr float DefaultWindowWidth = 600;
constexpr float DefaultWindowHeight = 800;

constexpr float ToolbarHeight = 30;

}

InspectorWindow::InspectorWindow(const std::shared_ptr<Window>& target_window) : 
    target_window_(target_window) {

    ZAF_EXPECT(target_window);
}


void InspectorWindow::Initialize() {

    __super::Initialize();

    SetTitle(L"Inspector -" + target_window_->Title());
    SetIsPopup(true);
    SetCanMinimize(false);
    SetOwner(target_window_);

    zaf::Rect rect;
    rect.size.width = DefaultWindowWidth;
    rect.size.height = DefaultWindowHeight;

    auto target_window_rect = target_window_->Rect();
    rect.position.x = target_window_rect.position.x + target_window_rect.size.width;
    rect.position.y = target_window_rect.position.y;

    SetRect(rect);
    SetInitialRectStyle(InitialRectStyle::Custom);

    RootControl()->SetLayouter(Create<VerticalLayouter>());

    InitializeSplitControl();

    InitializeFirstPaneToolbar();
    InitializeTreeControl();

    InitializeSecondPaneToolbar();
    InitializePropertyGrid();
}


void InspectorWindow::InitializeSplitControl() {

    split_control_ = Create<SplitControl>();
    split_control_->SetIsHorizontalSplit(true);
    split_control_->FirstPane()->SetLayouter(Create<VerticalLayouter>());
    split_control_->SecondPane()->SetLayouter(Create<VerticalLayouter>());

    RootControl()->AddChild(split_control_);
}


void InspectorWindow::InitializeFirstPaneToolbar() {

    auto select_button = Create<Button>();
    select_button->SetText(L"Select");
    Disposables() += select_button->ClickEvent().Subscribe(std::bind([this]() {
        target_window_->BeginSelectInspectedControl();
    }));

    auto toolbar = Create<Control>();
    toolbar->SetFixedHeight(ToolbarHeight);
    toolbar->SetLayouter(Create<HorizontalLayouter>());
    toolbar->AddChild(select_button);

    split_control_->FirstPane()->AddChild(toolbar);
}


void InspectorWindow::InitializeTreeControl() {

    tree_control_ = Create<TreeControl>();
    tree_control_->SetBorder(Frame(0));
    tree_control_->SetAutoHideScrollBars(true);

    data_source_ = Create<internal::InspectDataSource>(target_window_);
    tree_control_->SetDelegate(std::dynamic_pointer_cast<TreeControlDelegate>(shared_from_this()));
    tree_control_->SetDataSource(data_source_);

    tree_control_->ExpandItem(target_window_);
    tree_control_->ExpandItem(target_window_->RootControl());

    Disposables() += tree_control_->SelectionChangeEvent().Subscribe(std::bind([this]() {

        auto selected_item = tree_control_->GetFirstSelectedItem();
        if (!selected_item) {
            return;
        }

        if (selected_item == property_grid_->TargetObject()) {
            return;
        }

        //Retain expanded properties after switching target control.
        auto expanded_node_tree = property_grid_->GetExpandedNodeTree();
        property_grid_->SetTargetObject(selected_item);
        property_grid_->ExpandNodeTree(expanded_node_tree);
    }));

    split_control_->FirstPane()->AddChild(tree_control_);
}


void InspectorWindow::InitializeSecondPaneToolbar() {

    auto refresh_button = Create<Button>();
    refresh_button->SetText(L"Refresh");
    Disposables() += refresh_button->ClickEvent().Subscribe(std::bind([this]() {
        property_grid_->RefreshValues();
    }));

    auto toolbar = Create<Control>();
    toolbar->SetFixedHeight(ToolbarHeight);
    toolbar->SetLayouter(Create<HorizontalLayouter>());
    toolbar->AddChild(refresh_button);

    split_control_->SecondPane()->AddChild(toolbar);
}


void InspectorWindow::InitializePropertyGrid() {

    property_grid_ = Create<PropertyGrid>();
    property_grid_->SetBorder(zaf::Frame{});
    property_grid_->SetAutoHideScrollBars(true);

    split_control_->SecondPane()->AddChild(property_grid_);
}


float InspectorWindow::EstimateItemHeight(
    const std::shared_ptr<Object>& parent_item_data,
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    return 30;
}


std::shared_ptr<TreeItem> InspectorWindow::CreateItem(
    const std::shared_ptr<Object>& parent_item_data,
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    std::shared_ptr<internal::InspectItem> item;
    auto control = std::dynamic_pointer_cast<Control>(item_data);
    if (control) {
        item = Create<internal::InspectControlItem>(control);
    }
    else {
        item = Create<internal::InspectItem>();
    }

    item->SetIsHighlight(item_data == highlight_object_);

    Disposables() += item->MouseEnterEvent().Subscribe(
        [this, item_data](const MouseEnterInfo& event_info) {

        ChangeHighlightObject(item_data);

        auto control = std::dynamic_pointer_cast<Control>(item_data);
        if (control) {
            target_window_->SetHighlightControl(control);
        }
    });

    Disposables() += item->MouseLeaveEvent().Subscribe(
        [this, item_data](const MouseLeaveInfo& event_info) {

        if (highlight_object_ != item_data) {
            return;
        }

        ChangeHighlightObject(nullptr);
        target_window_->SetHighlightControl(nullptr);
    });

    return item;
}


void InspectorWindow::HighlightControl(const std::shared_ptr<Control>& control) {

    std::vector<std::shared_ptr<Object>> parent_chain;
    auto parent = control->Parent();
    while (parent) {
        parent_chain.push_back(parent);
        parent = parent->Parent();
    }
    std::reverse(parent_chain.begin(), parent_chain.end());

    parent_chain.push_back(target_window_);

    auto update_guard = tree_control_->BeginUpdate();

    for (const auto& each_parent : parent_chain) {
        tree_control_->ExpandItem(each_parent);
    }

    tree_control_->ScrollToItem(control);
    ChangeHighlightObject(control);
}


void InspectorWindow::ChangeHighlightObject(const std::shared_ptr<Object>& object) {

    if (highlight_object_ == object) {
        return;
    }

    auto old_highlight_object = highlight_object_;
    highlight_object_ = object;

    if (old_highlight_object) {
        tree_control_->ReloadItem(old_highlight_object);
    }

    if (highlight_object_) {
        tree_control_->ReloadItem(highlight_object_);
    }
}


void InspectorWindow::SelectControl(const std::shared_ptr<Control>& control) {
    tree_control_->SelectItem(control);
}


void InspectorWindow::ControlAddChild(
    const std::shared_ptr<Control>& parent,
    std::size_t added_index) {

    data_source_->ControlAddChild(parent, added_index);
}


void InspectorWindow::ControlRemoveChild(
    const std::shared_ptr<Control>& parent,
    std::size_t removed_index) {

    data_source_->ControlRemoveChild(parent, removed_index);
}


std::shared_ptr<internal::InspectorPort> InspectorWindow::GetPort() {
    return std::dynamic_pointer_cast<internal::InspectorPort>(shared_from_this());
}

}