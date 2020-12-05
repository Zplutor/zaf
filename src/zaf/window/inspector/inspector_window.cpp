#include <zaf/window/inspector/inspector_window.h>
#include <zaf/base/error/check.h>
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
constexpr float DefaultWindowHeight = 600;

}

InspectorWindow::InspectorWindow(const std::shared_ptr<Window>& target_window) : 
    target_window_(target_window) {

    ZAF_EXPECT(target_window);
}


void InspectorWindow::Initialize() {

    __super::Initialize();

    SetTitle(L"Inspector -" + target_window_->GetTitle());
    SetIsPopup(true);
    SetCanMinimize(false);
    SetOwner(target_window_);

    Rect rect;
    rect.size.width = DefaultWindowWidth;
    rect.size.height = DefaultWindowHeight;

    auto target_window_rect = target_window_->GetRect();
    rect.position.x = target_window_rect.position.x + target_window_rect.size.width;
    rect.position.y = target_window_rect.position.y;

    SetRect(rect);
    SetInitialRectStyle(InitialRectStyle::Custom);

    GetRootControl()->SetLayouter(Create<VerticalLayouter>());

    InitializeToolbar();
    InitializeTreeControl();
}


void InspectorWindow::InitializeToolbar() {

    auto select_inspector_control_button = Create<Button>();
    select_inspector_control_button->SetText(L"Select");
    Subscriptions() += select_inspector_control_button->ClickEvent().Subscribe(std::bind([this]() {
        target_window_->BeginSelectInspectedControl();
    }));

    auto toolbar = Create<Control>();
    toolbar->SetFixedHeight(40);
    toolbar->SetLayouter(Create<HorizontalLayouter>());
    toolbar->AddChild(select_inspector_control_button);

    GetRootControl()->AddChild(toolbar);
}


void InspectorWindow::InitializeTreeControl() {

    tree_control_ = Create<TreeControl>();

    data_source_ = Create<internal::InspectDataSource>(target_window_);
    tree_control_->SetDataSource(data_source_);
    tree_control_->SetDelegate(std::dynamic_pointer_cast<TreeControlDelegate>(shared_from_this()));

    GetRootControl()->AddChild(tree_control_);
}


float InspectorWindow::EstimateItemHeight(
    const std::shared_ptr<Object>& parent_item_data,
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    return 30;
}


std::wstring InspectorWindow::GetItemText(
    const std::shared_ptr<Object>& parent_item_data,
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    return item_data->ToString();
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

    Subscriptions() += item->MouseEnterEvent().Subscribe(
        [this, item_data](const ControlMouseEnterInfo& event_info) {

        ChangeHighlightObject(item_data);

        auto control = std::dynamic_pointer_cast<Control>(item_data);
        if (control) {
            target_window_->SetHighlightControl(control);
        }
    });

    Subscriptions() += item->MouseLeaveEvent().Subscribe(
        [this, item_data](const ControlMouseLeaveInfo& event_info) {

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
    auto parent = control->GetParent();
    while (parent) {
        parent_chain.push_back(parent);
        parent = parent->GetParent();
    }
    std::reverse(parent_chain.begin(), parent_chain.end());

    parent_chain.push_back(target_window_);

    Control::UpdateGuard update_guard(*tree_control_);

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

}


void InspectorWindow::ControlAddChild(const std::shared_ptr<Control>& parent) {

    data_source_->ControlAddChild(parent);
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