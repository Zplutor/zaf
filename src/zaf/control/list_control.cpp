#include <zaf/control/list_control.h>
#include <algorithm>
#include <zaf/base/error/contract_error.h>
#include <zaf/control/internal/list_control/list_control_implementation.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include <zaf/control/list_item_container.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>
#include <zaf/rx/subject.h>

namespace zaf {

ZAF_OBJECT_IMPL(ListControl);


ListControl::ListControl() : 
    implementation_(std::make_shared<internal::ListControlImplementation>(*this)){

}


ListControl::~ListControl() {
 
}


void ListControl::Initialize() {

    __super::Initialize();

    item_container_ = Create<ListItemContainer>();

    internal::ListControlImplementation::InitializeParameters initialize_parameters;
    initialize_parameters.item_container = item_container_;

    initialize_parameters.data_source_change_event = 
        std::bind(&ListControl::OnDataSourceChanged, this, std::placeholders::_1);
    initialize_parameters.delegate_change_event = 
        std::bind(&ListControl::OnDelegateChanged, this, std::placeholders::_1);
    initialize_parameters.item_container_change_event = 
        std::bind(&ListControl::OnItemContainerChanged, this, std::placeholders::_1);
    initialize_parameters.selection_change_event = 
        std::bind(&ListControl::OnCoreSelectionChanged, this);
    initialize_parameters.item_double_click_event = 
        std::bind(&ListControl::OnItemDoubleClick, this, std::placeholders::_1);

    implementation_->Initialize(initialize_parameters);
}


void ListControl::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    implementation_->OnLayout();
}


void ListControl::OnVerticalScrollBarChanged(
    const std::shared_ptr<ScrollBar>& previous_scroll_bar) {

    __super::OnVerticalScrollBarChanged(previous_scroll_bar);

    implementation_->OnVerticalScrollBarChange(previous_scroll_bar);
}


void ListControl::SetDataSource(const std::weak_ptr<ListDataSource>& data_source) {

    data_source_ = data_source;
    implementation_->SetDataSource(data_source_);
}


void ListControl::SetDelegate(const std::weak_ptr<ListControlDelegate>& delegate) {

    delegate_ = delegate;
    implementation_->SetDelegate(delegate_);
}


void ListControl::SetItemContainer(const std::shared_ptr<ListItemContainer>& item_container) {

    ZAF_EXPECT(item_container);

    item_container_ = item_container;
    implementation_->SetItemContainer(item_container_);
}


void ListControl::Reload() {
    implementation_->Reload();
}


std::size_t ListControl::GetItemCount() const {
    return implementation_->GetItemCount();
}


std::shared_ptr<Object> ListControl::GetItemDataAtIndex(std::size_t index) const {

    auto data_source = data_source_.lock();
    if (data_source) {
        return data_source->GetDataAtIndex(index);
    }
    return Create<Object>();
}


std::shared_ptr<ListItem> ListControl::GetVisibleItemAtIndex(std::size_t index) const noexcept {
    return implementation_->GetVisibleItemAtIndex(index);
}


bool ListControl::AutoAdjustScrollBarSmallChange() const {
    return implementation_->AutoAdjustScrollBarSmallChange();
}

void ListControl::SetAutoAdjustScrollBarSmallChange(bool value) {
    implementation_->SetAutoAdjustScrollBarSmallChange(value);
}


SelectionMode ListControl::SelectionMode() const {
    return implementation_->GetSelectionMode();
}

void ListControl::SetSelectionMode(zaf::SelectionMode selection_mode) {
    implementation_->SetSelectionMode(selection_mode);
}


void ListControl::SelectItemAtIndex(std::size_t index) {
    implementation_->SelectItemAtIndex(index);
}


void ListControl::UnselectItemAtIndex(std::size_t index) {
    implementation_->UnselectItemAtIndex(index);
}


void ListControl::SelectAllItems() {
    implementation_->SelectAllItems();
}


void ListControl::UnselectAllItems() {
    implementation_->UnselectAllItems();
}


std::size_t ListControl::GetSelectedItemCount() const {
    return implementation_->GetSelectedItemCount();
}


std::vector<std::size_t> ListControl::GetAllSelectedItemIndexes() const {
    return implementation_->GetAllSelectedItemIndexes();
}


std::optional<std::size_t> ListControl::FirstSelectedItemIndex() const noexcept {
    return implementation_->GetFirstSelectedItemIndex();
}


std::shared_ptr<Object> ListControl::FirstSelectedItemData() const {

    auto index = FirstSelectedItemIndex();
    if (!index) {
        return nullptr;
    }

    return GetItemDataAtIndex(*index);
}


bool ListControl::IsItemSelectedAtIndex(std::size_t index) const {
    return implementation_->IsItemSelectedAtIndex(index);
}


void ListControl::ScrollToItemAtIndex(std::size_t index) {
    implementation_->ScrollToItemAtIndex(index);
}


std::optional<std::size_t> ListControl::FindItemIndexAtPosition(const Point& position) const {
    return implementation_->FindItemIndexAtPosition(position);
}


void ListControl::OnCoreSelectionChanged() {

    ListControlSelectionChangedInfo event_info{ zaf::As<ListControl>(shared_from_this()) };
    OnSelectionChanged(event_info);
}


void ListControl::OnSelectionChanged(const ListControlSelectionChangedInfo& event_info) {
    selection_changed_event_.Raise(event_info);
}


Observable<ListControlSelectionChangedInfo> ListControl::SelectionChangedEvent() const {
    return selection_changed_event_.GetObservable();
}


void ListControl::OnItemDoubleClick(std::size_t item_index) {

    item_double_click_event_.Raise(ListControlItemDoubleClickInfo{
        As<ListControl>(shared_from_this()),
        item_index
    });
}


Observable<ListControlItemDoubleClickInfo> ListControl::ItemDoubleClickEvent() const {
    return item_double_click_event_.GetObservable();
}

}