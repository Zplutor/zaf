#include <zaf/control/list_control.h>
#include <algorithm>
#include <zaf/base/error/contract_error.h>
#include <zaf/internal/list/list_control_core.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include <zaf/control/list_item_container.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/creation.h>
#include <zaf/internal/list/list_control_parts_context.h>
#include <zaf/internal/theme.h>
#include <zaf/rx/subject.h>

namespace zaf {

ZAF_OBJECT_IMPL(ListControl);

ListControl::ListControl() : parts_(std::make_unique<internal::ListControlPartsContext>(this)) {

}


ListControl::~ListControl() {
 
}


void ListControl::Initialize() {

    __super::Initialize();

    auto item_container = Create<ListItemContainer>();

    internal::ListControlCore::InitializeParameters init_params;
    init_params.item_container = item_container;

    init_params.data_source_change_event = 
        std::bind_front(&ListControl::OnDataSourceChanged, this);
    init_params.delegate_change_event = 
        std::bind_front(&ListControl::OnDelegateChanged, this);
    init_params.item_container_change_event = 
        std::bind_front(&ListControl::OnItemContainerChanged, this);
    init_params.selection_changed_event = 
        std::bind(&ListControl::OnCoreSelectionChanged, this);
    init_params.item_double_click_event = 
        std::bind_front(&ListControl::OnCoreItemDoubleClick, this);
    init_params.context_menu_event =
        std::bind_front(&ListControl::OnCoreContextMenu, this);

    parts_->Core().Initialize(init_params);
}


void ListControl::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    parts_->Core().OnLayout();
}


void ListControl::OnMouseDown(const MouseDownInfo& event_info) {
    __super::OnMouseDown(event_info);
    parts_->InputHandler().HandleMouseDownEvent(event_info);
}


void ListControl::OnMouseMove(const MouseMoveInfo& event_info) {
    __super::OnMouseMove(event_info);
    parts_->InputHandler().HandleMouseMoveEvent(event_info);
}


void ListControl::OnMouseUp(const MouseUpInfo& event_info) {
    __super::OnMouseUp(event_info);
    parts_->InputHandler().HandleMouseUpEvent(event_info);
}


void ListControl::OnKeyDown(const KeyDownInfo& event_info) {
    __super::OnKeyDown(event_info);
    parts_->InputHandler().HandleKeyDownEvent(event_info);
}


void ListControl::OnVerticalScrollBarChanged(
    const std::shared_ptr<ScrollBar>& previous_scroll_bar) {

    __super::OnVerticalScrollBarChanged(previous_scroll_bar);

    parts_->Core().OnVerticalScrollBarChange();
}


void ListControl::SetDataSource(const std::weak_ptr<ListDataSource>& data_source) {

    data_source_ = data_source;
    parts_->Core().SetDataSource(data_source_);
}


void ListControl::SetDelegate(const std::weak_ptr<ListControlDelegate>& delegate) {

    delegate_ = delegate;
    parts_->Core().SetDelegate(delegate_);
}


const std::shared_ptr<ListItemContainer>& ListControl::ItemContainer() const noexcept {
    return parts_->Core().ItemContainer();
}

void ListControl::SetItemContainer(std::shared_ptr<ListItemContainer> item_container) {

    ZAF_EXPECT(item_container);
    parts_->Core().SetItemContainer(std::move(item_container));
}


void ListControl::Reload() {
    parts_->Core().Reload();
}


std::size_t ListControl::ItemCount() const {
    return parts_->Core().GetItemCount();
}


std::shared_ptr<Object> ListControl::GetItemDataAtIndex(std::size_t index) const {

    auto data_source = data_source_.lock();
    if (data_source) {
        return data_source->GetDataAtIndex(index);
    }
    return Create<Object>();
}


std::shared_ptr<ListItem> ListControl::GetVisibleItemAtIndex(std::size_t index) const noexcept {
    return parts_->VisibleItemManager().GetVisibleItemAtIndex(index);
}


bool ListControl::AutoAdjustScrollBarSmallChange() const {
    return parts_->Core().AutoAdjustScrollBarSmallChange();
}

void ListControl::SetAutoAdjustScrollBarSmallChange(bool value) {
    parts_->Core().SetAutoAdjustScrollBarSmallChange(value);
}


SelectionMode ListControl::SelectionMode() const {
    return parts_->SelectionManager().SelectionMode();
}

void ListControl::SetSelectionMode(zaf::SelectionMode selection_mode) {
    parts_->SelectionManager().SetSelectionMode(selection_mode);
}


void ListControl::SelectItemAtIndex(std::size_t index) {
    parts_->SelectionManager().SelectItemAtIndex(index);
}


void ListControl::UnselectItemAtIndex(std::size_t index) {
    parts_->SelectionManager().UnselectItemAtIndex(index);
}


void ListControl::SelectAllItems() {
    parts_->SelectionManager().SelectAllItems();
}


void ListControl::UnselectAllItems() {
    parts_->SelectionManager().UnselectAllItems();
}


std::size_t ListControl::SelectedItemCount() const {
    return parts_->SelectionStore().GetAllSelectedCount();
}


std::vector<std::size_t> ListControl::SelectedItemIndexes() const {
    return parts_->SelectionStore().GetAllSelectedIndexes();
}


std::optional<std::size_t> ListControl::FirstSelectedItemIndex() const noexcept {
    return parts_->SelectionStore().GetFirstSelectedIndex();
}


std::shared_ptr<Object> ListControl::FirstSelectedItemData() const {

    auto index = FirstSelectedItemIndex();
    if (!index) {
        return nullptr;
    }

    return GetItemDataAtIndex(*index);
}


bool ListControl::IsItemSelectedAtIndex(std::size_t index) const {
    return parts_->SelectionStore().IsIndexSelected(index);
}


void ListControl::ScrollToItemAtIndex(std::size_t index) {
    parts_->Core().ScrollToItemAtIndex(index);
}


std::optional<std::size_t> ListControl::FindItemIndexAtPosition(const Point& position) const {
    return parts_->Core().FindItemIndexAtPosition(position);
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