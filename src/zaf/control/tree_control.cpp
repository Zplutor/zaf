#include <zaf/control/tree_control.h>
#include <zaf/base/error/check.h>
#include <zaf/base/event_utility.h>
#include <zaf/control/tree_control_delegate.h>
#include <zaf/control/tree_data_source.h>
#include <zaf/control/internal/tree_control/tree_control_implementation.h>
#include <zaf/reflection/reflection_type_definition.h>
#include <zaf/rx/subject.h>

namespace zaf {
namespace {

constexpr wchar_t* const kItemCollapseEventProperyName = L"ItemCollapseEvent";
constexpr wchar_t* const kItemExpandEventPropertyName = L"ItemExpandEvent";
constexpr wchar_t* const kSelectionChangeEventPropertyName = L"SelectionChangeEvent";

}

ZAF_DEFINE_REFLECTION_TYPE(TreeControl)
    //ZAF_DEFINE_PARSER(ListControlParser)
ZAF_DEFINE_END


TreeControl::TreeControl() :
    data_source_(this),
    delegate_(this),
    implementation_(std::make_shared<internal::TreeControlImplementation>(*this)) {

}


TreeControl::~TreeControl() {

}


void TreeControl::Initialize() {

    __super::Initialize();

    item_container_ = Create<ListItemContainer>();
    data_source_ = Create<TreeDataSource>();
    delegate_ = Create<TreeControlDelegate>();

    internal::TreeControlImplementation::InitializeParameters initialize_parameters;
    initialize_parameters.item_container = item_container_;
    initialize_parameters.data_source = data_source_.GetSharedPointer();
    initialize_parameters.delegate = delegate_.GetSharedPointer();
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


void TreeControl::Layout(const Rect& previous_rect) {

    __super::Layout(previous_rect);

    implementation_->GetListImplementation().OnLayout();
}


void TreeControl::SetDataSource(const std::shared_ptr<TreeDataSource>& data_source) {

    ZAF_EXPECT(data_source);

    data_source_ = data_source;
    implementation_->SetDataSource(data_source_.GetSharedPointer());
}


void TreeControl::SetDelegate(const std::shared_ptr<TreeControlDelegate>& delegate) {

    ZAF_EXPECT(delegate);

    delegate_ = delegate;
    implementation_->SetDelegate(delegate_.GetSharedPointer());
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


void TreeControl::SelectionChange() {

    auto observer = GetEventObserver<TreeControlSelectionChangeInfo>(
        GetPropertyMap(), 
        kSelectionChangeEventPropertyName);

    if (!observer) {
        return;
    }

    TreeControlSelectionChangeInfo event_info;
    event_info.tree_control = std::dynamic_pointer_cast<TreeControl>(shared_from_this());
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

    TreeControlItemExpandInfo event_info;
    event_info.tree_control = std::dynamic_pointer_cast<TreeControl>(shared_from_this());
    event_info.item_data = data;
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

    TreeControlItemCollapseInfo event_info;
    event_info.tree_control = std::dynamic_pointer_cast<TreeControl>(shared_from_this());
    event_info.item_data = data;
    observer->OnNext(event_info);
}


Observable<TreeControlItemCollapseInfo> TreeControl::ItemCollapseEvent() {

    return GetEventObservable<TreeControlItemCollapseInfo>(
        GetPropertyMap(),
        kItemCollapseEventProperyName);
}

}