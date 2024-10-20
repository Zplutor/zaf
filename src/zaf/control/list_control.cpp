#include <zaf/control/list_control.h>
#include <algorithm>
#include <zaf/base/error/contract_error.h>
#include <zaf/internal/list/list_control_core.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include <zaf/control/list_item_container.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/creation.h>
#include <zaf/internal/list/list_control_part_context.h>
#include <zaf/internal/theme.h>
#include <zaf/rx/subject.h>

namespace zaf {

ZAF_OBJECT_IMPL(ListControl);


ListControl::ListControl() : core_(std::make_shared<internal::ListControlCore>(*this)){

}


ListControl::~ListControl() {
 
}


void ListControl::Initialize() {

    __super::Initialize();

    item_container_ = Create<ListItemContainer>();

    internal::ListControlCore::InitializeParameters init_params;
    init_params.item_container = item_container_;

    init_params.data_source_change_event = 
        std::bind_front(&ListControl::OnDataSourceChanged, this);
    init_params.delegate_change_event = 
        std::bind_front(&ListControl::OnDelegateChanged, this);
    init_params.item_container_change_event = 
        std::bind_front(&ListControl::OnItemContainerChanged, this);
    init_params.selection_change_event = 
        std::bind(&ListControl::OnCoreSelectionChanged, this);
    init_params.item_double_click_event = 
        std::bind_front(&ListControl::OnCoreItemDoubleClick, this);
    init_params.context_menu_event =
        std::bind_front(&ListControl::OnCoreContextMenu, this);

    core_->Initialize(init_params);
}


void ListControl::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    core_->OnLayout();
}


void ListControl::OnMouseDown(const MouseDownInfo& event_info) {
    __super::OnMouseDown(event_info);
    core_->PartContext().InputHandler().HandleMouseDownEvent(event_info);
}


void ListControl::OnMouseMove(const MouseMoveInfo& event_info) {
    __super::OnMouseMove(event_info);
    core_->PartContext().InputHandler().HandleMouseMoveEvent(event_info);
}


void ListControl::OnMouseUp(const MouseUpInfo& event_info) {
    __super::OnMouseUp(event_info);
    core_->PartContext().InputHandler().HandleMouseUpEvent(event_info);
}


void ListControl::OnKeyDown(const KeyDownInfo& event_info) {
    __super::OnKeyDown(event_info);
    core_->PartContext().InputHandler().HandleKeyDownEvent(event_info);
}


void ListControl::OnVerticalScrollBarChanged(
    const std::shared_ptr<ScrollBar>& previous_scroll_bar) {

    __super::OnVerticalScrollBarChanged(previous_scroll_bar);

    core_->OnVerticalScrollBarChange(previous_scroll_bar);
}


void ListControl::SetDataSource(const std::weak_ptr<ListDataSource>& data_source) {

    data_source_ = data_source;
    core_->SetDataSource(data_source_);
}


void ListControl::SetDelegate(const std::weak_ptr<ListControlDelegate>& delegate) {

    delegate_ = delegate;
    core_->SetDelegate(delegate_);
}


void ListControl::SetItemContainer(std::shared_ptr<ListItemContainer> item_container) {

    ZAF_EXPECT(item_container);

    item_container_ = std::move(item_container);
    core_->SetItemContainer(item_container_);
}


void ListControl::Reload() {
    core_->Reload();
}


std::size_t ListControl::ItemCount() const {
    return core_->GetItemCount();
}


std::shared_ptr<Object> ListControl::GetItemDataAtIndex(std::size_t index) const {

    auto data_source = data_source_.lock();
    if (data_source) {
        return data_source->GetDataAtIndex(index);
    }
    return Create<Object>();
}


std::shared_ptr<ListItem> ListControl::GetVisibleItemAtIndex(std::size_t index) const noexcept {
    return core_->GetVisibleItemAtIndex(index);
}


bool ListControl::AutoAdjustScrollBarSmallChange() const {
    return core_->AutoAdjustScrollBarSmallChange();
}

void ListControl::SetAutoAdjustScrollBarSmallChange(bool value) {
    core_->SetAutoAdjustScrollBarSmallChange(value);
}


SelectionMode ListControl::SelectionMode() const {
    return core_->GetSelectionMode();
}

void ListControl::SetSelectionMode(zaf::SelectionMode selection_mode) {
    core_->SetSelectionMode(selection_mode);
}


void ListControl::SelectItemAtIndex(std::size_t index) {
    core_->SelectItemAtIndex(index);
}


void ListControl::UnselectItemAtIndex(std::size_t index) {
    core_->UnselectItemAtIndex(index);
}


void ListControl::SelectAllItems() {
    core_->PartContext().SelectionManager().SelectAllItems();
}


void ListControl::UnselectAllItems() {
    core_->UnselectAllItems();
}


std::size_t ListControl::SelectedItemCount() const {
    return core_->GetSelectedItemCount();
}


std::vector<std::size_t> ListControl::SelectedItemIndexes() const {
    return core_->GetAllSelectedItemIndexes();
}


std::optional<std::size_t> ListControl::FirstSelectedItemIndex() const noexcept {
    return core_->GetFirstSelectedItemIndex();
}


std::shared_ptr<Object> ListControl::FirstSelectedItemData() const {

    auto index = FirstSelectedItemIndex();
    if (!index) {
        return nullptr;
    }

    return GetItemDataAtIndex(*index);
}


bool ListControl::IsItemSelectedAtIndex(std::size_t index) const {
    return core_->IsItemSelectedAtIndex(index);
}


void ListControl::ScrollToItemAtIndex(std::size_t index) {
    core_->ScrollToItemAtIndex(index);
}


std::optional<std::size_t> ListControl::FindItemIndexAtPosition(const Point& position) const {
    return core_->FindItemIndexAtPosition(position);
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


void ListControl::OnCoreItemDoubleClick(std::size_t item_index) {

    item_double_click_event_.Raise(ListControlItemDoubleClickInfo{
        As<ListControl>(shared_from_this()),
        item_index
    });
}


Observable<ListControlItemDoubleClickInfo> ListControl::ItemDoubleClickEvent() const {
    return item_double_click_event_.GetObservable();
}


std::shared_ptr<PopupMenu> ListControl::OnCoreContextMenu(
    std::optional<std::size_t> item_index,
    const std::shared_ptr<Object>& item_data) {

    ListControlContextMenuInfo event_info{ 
        As<ListControl>(shared_from_this()),
        item_index,
        item_data,
    };
    OnContextMenu(event_info);
    return event_info.Menu();
}


void ListControl::OnContextMenu(const ListControlContextMenuInfo& event_info) {
    context_menu_event_.Raise(event_info);
}


Observable<ListControlContextMenuInfo> ListControl::ContextMenuEvent() const {
    return context_menu_event_.GetObservable();
}

}