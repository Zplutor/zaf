#include <zaf/control/tree_control.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/internal/tree/tree_core.h>
#include <zaf/control/tree_control_delegate.h>
#include <zaf/control/tree_data_source.h>
#include <zaf/control/tree_item_container.h>
#include <zaf/internal/list/list_control_parts_context.h>
#include <zaf/rx/subject.h>

namespace zaf {

ZAF_OBJECT_IMPL(TreeControl)


TreeControl::TreeControl() : core_(std::make_shared<internal::TreeCore>(*this)) {

}


TreeControl::~TreeControl() {

}


void TreeControl::Initialize() {

    __super::Initialize();

    item_container_ = Create<TreeItemContainer>();
    data_source_.Assign(TreeDataSource::Empty(), this);
    delegate_.Assign(TreeControlDelegate::Default(), this);

    internal::TreeCore::InitializeParameters initialize_parameters;
    initialize_parameters.item_container = item_container_;
    initialize_parameters.data_source = data_source_.ToSharedPtr();
    initialize_parameters.delegate = delegate_.ToSharedPtr();

    initialize_parameters.data_source_change_event =
        std::bind(&TreeControl::OnDataSourceChanged, this, std::placeholders::_1);
    initialize_parameters.delegate_change_event =
        std::bind(&TreeControl::OnDelegateChanged, this, std::placeholders::_1);
    initialize_parameters.selection_change_event = std::bind(&TreeControl::SelectionChange, this);
    initialize_parameters.item_expand_event = 
        std::bind(&TreeControl::ItemExpand, this, std::placeholders::_1);
    initialize_parameters.item_collapse_event = 
        std::bind(&TreeControl::ItemCollapse, this, std::placeholders::_1);

    core_->Initialize(initialize_parameters);
}


void TreeControl::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    core_->ListParts().Core().OnLayout();
}


void TreeControl::OnMouseDown(const MouseDownInfo& event_info) {
    __super::OnMouseDown(event_info);
    core_->ListParts().InputHandler().HandleMouseDownEvent(event_info);
}


void TreeControl::OnMouseMove(const MouseMoveInfo& event_info) {
    __super::OnMouseMove(event_info);
    core_->ListParts().InputHandler().HandleMouseMoveEvent(event_info);
}


void TreeControl::OnMouseUp(const MouseUpInfo& event_info) {
    __super::OnMouseUp(event_info);
    core_->ListParts().InputHandler().HandleMouseUpEvent(event_info);
}


void TreeControl::OnKeyDown(const KeyDownInfo& event_info) {
    __super::OnKeyDown(event_info);
    core_->ListParts().InputHandler().HandleKeyDownEvent(event_info);
}


void TreeControl::OnFocusGained(const FocusGainedInfo& event_info) {
    __super::OnFocusGained(event_info);
    core_->ListParts().Core().HandleFocusGainedEvent(event_info);
}


void TreeControl::OnFocusLost(const FocusLostInfo& event_info) {
    __super::OnFocusLost(event_info);
    core_->ListParts().Core().HandleFocusLostEvent(event_info);
}



std::shared_ptr<TreeDataSource> TreeControl::DataSource() const noexcept {
    return data_source_.ToSharedPtr();
}

void TreeControl::SetDataSource(std::shared_ptr<TreeDataSource> data_source) {

    ZAF_EXPECT(data_source);

    data_source_.Assign(data_source, this);
    core_->SetDataSource(std::move(data_source));
}


std::shared_ptr<TreeControlDelegate> TreeControl::Delegate() const noexcept {
    return delegate_.ToSharedPtr();
}

void TreeControl::SetDelegate(std::shared_ptr<TreeControlDelegate> delegate) {

    ZAF_EXPECT(delegate);

    delegate_.Assign(delegate, this);
    core_->SetDelegate(std::move(delegate));
}


bool TreeControl::AutoAdjustScrollBarSmallChange() const {
    return core_->ListParts().Core().AutoAdjustScrollBarSmallChange();
}

void TreeControl::SetAutoAdjustScrollBarSmallChange(bool value) {
    core_->ListParts().Core().SetAutoAdjustScrollBarSmallChange(value);
}


void TreeControl::SetSelectionMode(ListSelectionMode selection_mode) {
    auto& list_parts = core_->ListParts();
    list_parts.SelectionManager().SetSelectionMode(selection_mode);
}


std::vector<std::shared_ptr<Object>> TreeControl::GetAllSelectedItems() const {
    return core_->GetAllSelectedItems();
}


std::shared_ptr<Object> TreeControl::GetFirstSelectedItem() const {
    return core_->GetFirstSelectedItem();
}


void TreeControl::SelectItem(const std::shared_ptr<Object>& data) {
    core_->SelectItem(data);
}


void TreeControl::UnselectItem(const std::shared_ptr<Object>& data) {
    core_->UnselectItem(data);
}


void TreeControl::ExpandItem(const std::shared_ptr<Object>& data) {
    core_->ExpandItem(data);
}


void TreeControl::CollapseItem(const std::shared_ptr<Object>& data) {
    core_->CollapseItem(data);
}


void TreeControl::ScrollToItem(const std::shared_ptr<Object>& data) {
    core_->ScrollToItem(data);
}


void TreeControl::ReloadItem(const std::shared_ptr<Object>& data) {
    core_->ReloadItem(data);
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