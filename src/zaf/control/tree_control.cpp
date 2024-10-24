#include <zaf/control/tree_control.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/control/internal/tree_control/tree_control_implementation.h>
#include <zaf/control/tree_control_delegate.h>
#include <zaf/control/tree_data_source.h>
#include <zaf/control/tree_item_container.h>
#include <zaf/internal/list/list_control_parts_context.h>
#include <zaf/rx/subject.h>

namespace zaf {

ZAF_OBJECT_IMPL(TreeControl)


TreeControl::TreeControl() :
    implementation_(std::make_shared<internal::TreeControlImplementation>(*this)) {

}


TreeControl::~TreeControl() {

}


void TreeControl::Initialize() {

    __super::Initialize();

    item_container_ = Create<TreeItemContainer>();

    internal::TreeControlImplementation::InitializeParameters initialize_parameters;
    initialize_parameters.item_container = item_container_;
    initialize_parameters.data_source_change_event =
        std::bind(&TreeControl::OnDataSourceChanged, this, std::placeholders::_1);
    initialize_parameters.delegate_change_event =
        std::bind(&TreeControl::OnDelegateChanged, this, std::placeholders::_1);
    initialize_parameters.selection_change_event = std::bind(&TreeControl::SelectionChange, this);
    initialize_parameters.item_expand_event = 
        std::bind(&TreeControl::ItemExpand, this, std::placeholders::_1);
    initialize_parameters.item_collapse_event = 
        std::bind(&TreeControl::ItemCollapse, this, std::placeholders::_1);

    implementation_->Initialize(initialize_parameters);
}


void TreeControl::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    implementation_->ListParts().Core().OnLayout();
}


void TreeControl::SetDataSource(const std::weak_ptr<TreeDataSource>& data_source) {

    data_source_ = data_source;
    implementation_->SetDataSource(data_source_);
}


void TreeControl::SetDelegate(const std::weak_ptr<TreeControlDelegate>& delegate) {

    delegate_ = delegate;
    implementation_->SetDelegate(delegate_);
}


bool TreeControl::AutoAdjustScrollBarSmallChange() const {
    return implementation_->ListParts().Core().AutoAdjustScrollBarSmallChange();
}

void TreeControl::SetAutoAdjustScrollBarSmallChange(bool value) {
    implementation_->ListParts().Core().SetAutoAdjustScrollBarSmallChange(value);
}


void TreeControl::SetSelectionMode(SelectionMode selection_mode) {
    auto& list_parts = implementation_->ListParts();
    list_parts.SelectionManager().SetSelectionMode(selection_mode);
}


std::vector<std::shared_ptr<Object>> TreeControl::GetAllSelectedItems() const {
    return implementation_->GetAllSelectedItems();
}


std::shared_ptr<Object> TreeControl::GetFirstSelectedItem() const {
    return implementation_->GetFirstSelectedItem();
}


void TreeControl::SelectItem(const std::shared_ptr<Object>& data) {
    implementation_->SelectItem(data);
}


void TreeControl::UnselectItem(const std::shared_ptr<Object>& data) {
    implementation_->UnselectItem(data);
}


void TreeControl::ExpandItem(const std::shared_ptr<Object>& data) {
    implementation_->ExpandItem(data);
}


void TreeControl::CollapseItem(const std::shared_ptr<Object>& data) {
    implementation_->CollapseItem(data);
}


void TreeControl::ScrollToItem(const std::shared_ptr<Object>& data) {
    implementation_->ScrollToItem(data);
}


void TreeControl::ReloadItem(const std::shared_ptr<Object>& data) {
    implementation_->ReloadItem(data);
}


void TreeControl::SelectionChange() {

    TreeControlSelectionChangeInfo event_info(
        std::dynamic_pointer_cast<TreeControl>(shared_from_this()));

    selection_changed_event_.Raise(event_info);
}


Observable<TreeControlSelectionChangeInfo> TreeControl::SelectionChangeEvent() const {
    return selection_changed_event_.GetObservable();
}


void TreeControl::ItemExpand(const std::shared_ptr<Object>& data) {

    TreeControlItemExpandInfo event_info(
        std::dynamic_pointer_cast<TreeControl>(shared_from_this()),
        data);

    item_expand_event_.Raise(event_info);
}


Observable<TreeControlItemExpandInfo> TreeControl::ItemExpandEvent() const {
    return item_expand_event_.GetObservable();
}


void TreeControl::ItemCollapse(const std::shared_ptr<Object>& data) {

    TreeControlItemCollapseInfo event_info(
        std::dynamic_pointer_cast<TreeControl>(shared_from_this()), 
        data);

    item_collapse_event_.Raise(event_info);
}


Observable<TreeControlItemCollapseInfo> TreeControl::ItemCollapseEvent() const {
    return item_collapse_event_.GetObservable();
}

}