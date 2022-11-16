#include <zaf/control/tree_control.h>
#include <zaf/base/error/check.h>
#include <zaf/base/event_utility.h>
#include <zaf/control/tree_control_delegate.h>
#include <zaf/control/tree_data_source.h>
#include <zaf/control/internal/tree_control/tree_control_implementation.h>
#include <zaf/object/type_definition.h>
#include <zaf/rx/subject.h>

namespace zaf {
namespace {

constexpr const wchar_t* const kItemCollapseEventProperyName = L"ItemCollapseEvent";
constexpr const wchar_t* const kItemExpandEventPropertyName = L"ItemExpandEvent";
constexpr const wchar_t* const kSelectionChangeEventPropertyName = L"SelectionChangeEvent";

}

ZAF_DEFINE_TYPE(TreeControl)
ZAF_DEFINE_TYPE_END


TreeControl::TreeControl() :
    implementation_(std::make_shared<internal::TreeControlImplementation>(*this)) {

}


TreeControl::~TreeControl() {

}


void TreeControl::Initialize() {

    __super::Initialize();

    item_container_ = Create<ListItemContainer>();

    internal::TreeControlImplementation::InitializeParameters initialize_parameters;
    initialize_parameters.item_container = item_container_;
    initialize_parameters.data_source_change_event =
        std::bind(&TreeControl::DataSourceChange, this, std::placeholders::_1);
    initialize_parameters.delegate_change_event =
        std::bind(&TreeControl::DelegateChange, this, std::placeholders::_1);
    initialize_parameters.item_container_change_event =
        std::bind(&TreeControl::ItemContainerChange, this, std::placeholders::_1);
    initialize_parameters.selection_change_event = std::bind(&TreeControl::SelectionChange, this);
    initialize_parameters.item_expand_event = 
        std::bind(&TreeControl::ItemExpand, this, std::placeholders::_1);
    initialize_parameters.item_collapse_event = 
        std::bind(&TreeControl::ItemCollapse, this, std::placeholders::_1);

    implementation_->Initialize(initialize_parameters);
}


void TreeControl::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    implementation_->GetListImplementation().OnLayout();
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
    return implementation_->GetListImplementation().AutoAdjustScrollBarSmallChange();
}

void TreeControl::SetAutoAdjustScrollBarSmallChange(bool value) {
    implementation_->GetListImplementation().SetAutoAdjustScrollBarSmallChange(value);
}


void TreeControl::SetSelectionMode(SelectionMode selection_mode) {
    implementation_->GetListImplementation().SetSelectionMode(selection_mode);
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

    auto observer = GetEventObserver<TreeControlSelectionChangeInfo>(
        GetPropertyMap(), 
        kSelectionChangeEventPropertyName);

    if (!observer) {
        return;
    }

    TreeControlSelectionChangeInfo event_info(
        std::dynamic_pointer_cast<TreeControl>(shared_from_this()));
    observer->OnNext(event_info);
}


Observable<TreeControlSelectionChangeInfo> TreeControl::SelectionChangeEvent() {

    return GetEventObservable<TreeControlSelectionChangeInfo>(
        GetPropertyMap(),
        kSelectionChangeEventPropertyName);
}


void TreeControl::ItemExpand(const std::shared_ptr<Object>& data) {

    auto observer = GetEventObserver<TreeControlItemExpandInfo>(
        GetPropertyMap(),
        kItemExpandEventPropertyName);

    if (!observer) {
        return;
    }

    TreeControlItemExpandInfo event_info(
        std::dynamic_pointer_cast<TreeControl>(shared_from_this()),
        data);
    observer->OnNext(event_info);
}


Observable<TreeControlItemExpandInfo> TreeControl::ItemExpandEvent() {

    return GetEventObservable<TreeControlItemExpandInfo>(
        GetPropertyMap(),
        kItemExpandEventPropertyName);
}


void TreeControl::ItemCollapse(const std::shared_ptr<Object>& data) {

    auto observer = GetEventObserver<TreeControlItemCollapseInfo>(
        GetPropertyMap(),
        kItemCollapseEventProperyName);

    if (!observer) {
        return;
    }

    TreeControlItemCollapseInfo event_info(
        std::dynamic_pointer_cast<TreeControl>(shared_from_this()), 
        data);
    observer->OnNext(event_info);
}


Observable<TreeControlItemCollapseInfo> TreeControl::ItemCollapseEvent() {

    return GetEventObservable<TreeControlItemCollapseInfo>(
        GetPropertyMap(),
        kItemCollapseEventProperyName);
}

}