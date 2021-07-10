#include <zaf/control/list_control.h>
#include <algorithm>
#include <zaf/base/error/check.h>
#include <zaf/base/event_utility.h>
#include <zaf/control/internal/list_control/list_control_implementation.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include <zaf/control/list_item_container.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>
#include <zaf/parsing/parsers/list_control_parser.h>
#include <zaf/object/type_definition.h>
#include <zaf/rx/subject.h>
#include <zaf/serialization/properties.h>

namespace zaf {

static const wchar_t* const kSelectionChangeEventPropertyName = L"SelectionChangeEvent";

ZAF_DEFINE_TYPE(ListControl)
    ZAF_DEFINE_TYPE_PARSER(ListControlParser)
ZAF_DEFINE_TYPE_END


ListControl::ListControl() : 
    data_source_(this),
    delegate_(this),
    implementation_(std::make_shared<internal::ListControlImplementation>(*this)){

}


ListControl::~ListControl() {
 
}


void ListControl::Initialize() {

    __super::Initialize();

    item_container_ = Create<ListItemContainer>();
    data_source_ = Create<ListDataSource>();
    delegate_ = Create<ListControlDelegate>();

    internal::ListControlImplementation::InitializeParameters initialize_parameters;
    initialize_parameters.item_container = item_container_;
    initialize_parameters.data_source = data_source_.GetSharedPointer();
    initialize_parameters.delegate = delegate_.GetSharedPointer();

    initialize_parameters.data_source_change_event = 
        std::bind(&ListControl::DataSourceChange, this, std::placeholders::_1);
    initialize_parameters.delegate_change_event = 
        std::bind(&ListControl::DelegateChange, this, std::placeholders::_1);
    initialize_parameters.item_container_change_event = 
        std::bind(&ListControl::ItemContainerChange, this, std::placeholders::_1);
    initialize_parameters.selection_change_event = std::bind(&ListControl::SelectionChange, this);

    implementation_->Initialize(initialize_parameters);
}


void ListControl::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    implementation_->OnLayout();
}


void ListControl::VerticalScrollBarChange(const std::shared_ptr<ScrollBar>& previous_scroll_bar) {

    __super::VerticalScrollBarChange(previous_scroll_bar);

    implementation_->OnVerticalScrollBarChange(previous_scroll_bar);
}


void ListControl::SetDataSource(const std::shared_ptr<ListDataSource>& data_source) {

    ZAF_EXPECT(data_source);

    data_source_ = data_source;
    implementation_->SetDataSource(data_source_.GetSharedPointer());
}


void ListControl::SetDelegate(const std::shared_ptr<ListControlDelegate>& delegate) {

    ZAF_EXPECT(delegate);

    delegate_ = delegate;
    implementation_->SetDelegate(delegate_.GetSharedPointer());
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
    return data_source_->GetDataAtIndex(index);
}


bool ListControl::AutoAdjustScrollBarSmallChange() const {
    return implementation_->AutoAdjustScrollBarSmallChange();
}

void ListControl::SetAutoAdjustScrollBarSmallChange(bool value) {
    implementation_->SetAutoAdjustScrollBarSmallChange(value);
}


SelectionMode ListControl::GetSelectionMode() const {
    return implementation_->GetSelectionMode();
}

void ListControl::SetSelectionMode(SelectionMode selection_mode) {
    implementation_->SetSelectionMode(selection_mode);
}


Observable<ListControlSelectionChangeInfo> ListControl::SelectionChangeEvent() {

    return GetEventObservable<ListControlSelectionChangeInfo>(
        GetPropertyMap(),
        kSelectionChangeEventPropertyName);
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


void ListControl::OnIsEnabledChanged() {

    __super::OnIsEnabledChanged();

    if (! IsEnabled()) {
        UnselectAllItems();
    }
}


std::size_t ListControl::GetSelectedItemCount() const {
    return implementation_->GetSelectedItemCount();
}


std::vector<std::size_t> ListControl::GetAllSelectedItemIndexes() const {
    return implementation_->GetAllSelectedItemIndexes();
}


std::optional<std::size_t> ListControl::GetFirstSelectedItemIndex() const {
    return implementation_->GetFirstSelectedItemIndex();
}


std::shared_ptr<Object> ListControl::GetFirstSelectedItemData() const {

    auto index = GetFirstSelectedItemIndex();
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


void ListControl::SelectionChange() {

    auto observer = GetEventObserver<ListControlSelectionChangeInfo>(
        GetPropertyMap(), 
        kSelectionChangeEventPropertyName);

    if (!observer) {
        return;
    }

    ListControlSelectionChangeInfo event_info;
    event_info.list_control = std::dynamic_pointer_cast<ListControl>(shared_from_this());
    observer->OnNext(event_info);
}

}