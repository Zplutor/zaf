#include <zaf/window/inspector/inspector_window.h>
#include <zaf/base/error/check.h>
#include <zaf/control/layout/linear_layouter.h>
#include <zaf/control/tree_control.h>
#include <zaf/creation.h>
#include <zaf/window/inspector/internal/inspect_control_item.h>
#include <zaf/window/inspector/internal/inspect_data_source.h>
#include <zaf/window/inspector/internal/inspect_item.h>

namespace zaf {
namespace {

constexpr float DefaultWindowWidth = 400;
constexpr float DefaultWindowHeight = 400;

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

    InitializeTreeControl();
}


void InspectorWindow::InitializeTreeControl() {

    auto tree_control = Create<TreeControl>();

    data_source_ = Create<internal::InspectDataSource>(target_window_);
    tree_control->SetDataSource(data_source_);
    tree_control->SetDelegate(std::dynamic_pointer_cast<TreeControlDelegate>(shared_from_this()));

    auto root_control = GetRootControl();
    root_control->SetLayouter(Create<VerticalLayouter>());
    root_control->AddChild(tree_control);
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

    std::shared_ptr<TreeItem> item;

    auto control = std::dynamic_pointer_cast<Control>(item_data);
    if (control) {
        item = Create<internal::InspectControlItem>(control);
    }
    else {
        item = Create<internal::InspectItem>();
    }

    Subscriptions() += item->MouseEnterEvent().Subscribe(
        [this, item_data](const ControlMouseEnterInfo& event_info) {
    
        auto control = std::dynamic_pointer_cast<Control>(item_data);
        if (!control) {
            return;
        }

        inspected_control_ = control;
        target_window_->SetInspectedControl(inspected_control_);
    });

    Subscriptions() += item->MouseLeaveEvent().Subscribe(
        [this, item_data](const ControlMouseLeaveInfo& event_info) {

        auto control = std::dynamic_pointer_cast<Control>(item_data);
        if (!control) {
            return;
        }

        if (inspected_control_ == control) {
            inspected_control_ = nullptr;
            target_window_->SetInspectedControl(nullptr);
        }
    });

    return item;
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