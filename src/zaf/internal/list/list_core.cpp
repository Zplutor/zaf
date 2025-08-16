#include <zaf/internal/list/list_core.h>
#include <zaf/base/as.h>
#include <zaf/base/auto_reset.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/range.h>
#include <zaf/internal/list/list_control_parts_context.h>
#include <zaf/rx/disposable.h>

namespace zaf::internal {

void ListCore::Initialize(const InitializeParameters& parameters) {

    auto& owner = Parts().Owner();
    owner.SetCanFocus(true);
    owner.SetBackgroundColor(Color::White());
    owner.SetBorder(Frame(1));
    owner.SetBorderColor(Color::Black());

    //Item container must be the first.
    InstallItemContainer(parameters.item_container);
    InstallDataSource(parameters.data_source);
    InstallDelegate(parameters.delegate);

    RegisterScrollBarEvents();

    Disposables() += Parts().SelectionStore().ChangedEvent().Subscribe(
        std::bind_front(&ListCore::OnSelectionStoreChanged, this));

    Reload();
}


void ListCore::RegisterScrollBarEvents() {

    auto vertical_scroll_bar = Parts().Owner().VerticalScrollBar();

    vertical_scroll_bar_sub_ = vertical_scroll_bar->ScrollEvent().Subscribe(std::bind([this]() {
        Parts().VisibleItemManager().UpdateVisibleItems();
    }));
}


void ListCore::UnregisterScrollBarEvents() {
    if (vertical_scroll_bar_sub_) {
        vertical_scroll_bar_sub_->Dispose();
    }
}


void ListCore::AdjustScrollBarSmallChange() {

    if (!AutoAdjustScrollBarSmallChange()) {
        return;
    }

    auto delegate = delegate_.lock();
    if (!delegate) {
        return;
    }

    //Don't adjust small change if item height is variable.
    if (delegate->HasVariableItemHeight()) {
        return;
    }

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    if (data_source->GetDataCount() <= 0) {
        return;
    }

    auto item_data = data_source->GetDataAtIndex(0);
    auto item_height = delegate->EstimateItemHeight(0, item_data);

    auto vertical_scroll_bar = Parts().Owner().VerticalScrollBar();
    vertical_scroll_bar->SetSmallChange(static_cast<int>(item_height));
}


std::shared_ptr<ListDataSource> ListCore::DataSource() const noexcept {
    return data_source_.lock();
}


void ListCore::SetDataSource(std::weak_ptr<ListDataSource> data_source) {

    data_source_subs_.Clear();

    auto previous_data_source = data_source_;

    InstallDataSource(std::move(data_source));

    data_source_changed_event_.AsObserver().OnNext({
        previous_data_source.lock()
    });

    Reload();
}


void ListCore::InstallDataSource(std::weak_ptr<ListDataSource> data_source) {

    data_source_ = data_source;

    Parts().ItemHeightManager().ResetDataSource(data_source);

    RegisterDataSourceEvents();
}


void ListCore::RegisterDataSourceEvents() {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    data_source_subs_ += data_source->DataAddedEvent().Subscribe(
        std::bind_front(&ListCore::OnDataAdded, this));

    data_source_subs_ += data_source->DataRemovedEvent().Subscribe(
        std::bind_front(&ListCore::OnDataRemoved, this));

    data_source_subs_ += data_source->DataUpdatedEvent().Subscribe(
        std::bind_front(&ListCore::OnDataUpdated, this));

    data_source_subs_ += data_source->DataMovedEvent().Subscribe(
        std::bind_front(&ListCore::OnDataMoved, this));
}


rx::Observable<ListCoreDataSourceChangedInfo> ListCore::DataSourceChangedEvent() const {
    return data_source_changed_event_.AsObservable();
}


std::shared_ptr<ListControlDelegate> ListCore::Delegate() const noexcept {
    return delegate_.lock();
}


void ListCore::SetDelegate(std::weak_ptr<ListControlDelegate> delegate) {

    auto previous_delegate = delegate_;
    
    InstallDelegate(std::move(delegate));

    delegate_changed_event_.AsObserver().OnNext({
        previous_delegate.lock()
    });

    Reload();
}


void ListCore::InstallDelegate(std::weak_ptr<ListControlDelegate> delegate) {

    delegate_ = delegate;
    Parts().ItemHeightManager().ResetDelegate(delegate);
}


rx::Observable<ListCoreDelegateChangedInfo> ListCore::DelegateChangedEvent() const {
    return delegate_changed_event_.AsObservable();
}


std::shared_ptr<ListItemContainer> ListCore::ItemContainer() const noexcept {
    return item_container_.lock();
}


void ListCore::SetItemContainer(std::weak_ptr<ListItemContainer> item_container) {

    auto previous_item_container = item_container_;

    item_container_subs_.Clear();
    InstallItemContainer(std::move(item_container));

    item_container_changed_event_.AsObserver().OnNext({
        previous_item_container.lock()
    });

    Reload();
}


void ListCore::InstallItemContainer(std::weak_ptr<ListItemContainer> item_container) {

    auto container = item_container.lock();
    ZAF_EXPECT(container);

    item_container_ = container;

    item_container_subs_ += container->DoubleClickEvent().Subscribe(
        std::bind_front(&ListCore::OnItemContainerDoubleClick, this));

    Parts().Owner().SetScrollContent(container);
}


rx::Observable<ListCoreItemContainerChangedInfo> ListCore::ItemContainerChangedEvent() const {
    return item_container_changed_event_.AsObservable();
}


void ListCore::OnItemContainerDoubleClick(const DoubleClickInfo& event_info) {

    auto index = Parts().ItemHeightManager().GetItemIndex(event_info.Position().y);
    if (index) {
        item_double_click_event_.AsObserver().OnNext({ *index });
    }
}


void ListCore::OnLayout() {

    if (!disable_on_layout_) {
        Parts().VisibleItemManager().UpdateVisibleItems();
    }
}


void ListCore::OnVerticalScrollBarChange() {

    UnregisterScrollBarEvents();
    RegisterScrollBarEvents();
    AdjustScrollBarSmallChange();
}


void ListCore::HandleFocusGainedEvent(const FocusGainedInfo& event_info) {

    Parts().FocusManager().ChangeFocusByOwnerFocusGainedEvent(event_info);

    auto scroll_box = As<ScrollBox>(event_info.Source());
    if (scroll_box && scroll_box.get() == &Parts().Owner()) {
        RepaintSelectedItems();
    }
}


void ListCore::HandleFocusLostEvent(const FocusLostInfo& event_info) {

    Parts().FocusManager().ChangeFocusByOwnerFocusLostEvent(event_info);

    auto scroll_box = As<ScrollBox>(event_info.Source());
    if (scroll_box && scroll_box.get() == &Parts().Owner()) {
        RepaintSelectedItems();
    }
}


void ListCore::RepaintSelectedItems() {

    auto selected_indexes = Parts().SelectionStore().GetAllSelectedIndexes();

    auto& visible_item_manager = Parts().VisibleItemManager();
    for (auto each_index : selected_indexes) {

        auto visible_item = visible_item_manager.GetVisibleItemAtIndex(each_index);
        if (visible_item && visible_item->IsSelected()) {
            visible_item->NeedUpdateStyle();
            visible_item->NeedRepaint();
        }
    }
}


void ListCore::Reload() {
    InnerReload(false);
}


void ListCore::InnerReload(bool retain_state) {

    if (!retain_state) {

        //Remove the focused index.
        Parts().FocusManager().ChangeFocusByReloading();

        //Remove selected indexes.
        Parts().SelectionManager().UnselectAllItems();
    }

    //Remove all visible items.
    auto& visible_item_manager = Parts().VisibleItemManager();
    visible_item_manager.ClearVisibleItems();

    UpdateContentHeight();
    visible_item_manager.UpdateVisibleItems();
    AdjustScrollBarSmallChange();
}


void ListCore::UpdateContentHeight() {

    Parts().ItemHeightManager().ReloadItemHeights();
    SetScrollContentHeight(Parts().ItemHeightManager().GetTotalHeight());
}


void ListCore::OnDataAdded(const ListDataAddedInfo& event_info) {

    Parts().ItemHeightManager().OnDataAdded(event_info);

    if (is_handling_data_source_event_) {
        refresh_after_data_source_event_ = true;
        return;
    }

    auto auto_reset = MakeAutoReset(is_handling_data_source_event_, true);
    HandleDataAdded(event_info);
    RefreshItemsIfNeeded();
}


void ListCore::HandleDataAdded(const ListDataAddedInfo& event_info) {

    auto item_container = item_container_.lock();
    if (!item_container) {
        return;
    }

    //Adjust scroll bar small change if there is no items before adding.
    bool need_adjust_scroll_bar_small_change = 
        !Parts().VisibleItemManager().HasVisibleItem();

    auto update_guard = item_container->BeginUpdate();

    float position_difference = AdjustContentHeight();

    Parts().VisibleItemManager().HandleDataAdded(
        Range{ event_info.Index(), event_info.Count() },
        position_difference);

    Parts().FocusManager().ChangeFocusByAddingData(
        Range{ event_info.Index(), event_info.Count() });

    Parts().SelectionStore().AdjustSelectionByAddingIndexes(
        event_info.Index(),
        event_info.Count());

    if (need_adjust_scroll_bar_small_change) {
        AdjustScrollBarSmallChange();
    }
}


void ListCore::OnDataRemoved(const ListDataRemovedInfo& event_info) {

    Parts().ItemHeightManager().OnDataRemoved(event_info);

    if (is_handling_data_source_event_) {
        refresh_after_data_source_event_ = true;
        return;
    }

    auto auto_reset = MakeAutoReset(is_handling_data_source_event_, true);
    HandleDataRemoved(event_info);
    RefreshItemsIfNeeded();
}


void ListCore::HandleDataRemoved(const ListDataRemovedInfo& event_info) {

    auto item_container = item_container_.lock();
    if (!item_container) {
        return;
    }

    auto update_guard = item_container->BeginUpdate();

    float position_difference = AdjustContentHeight();

    Parts().VisibleItemManager().HandleDataRemoved(
        Range{ event_info.Index(), event_info.Count() },
        position_difference);

    Parts().FocusManager().ChangeFocusByRemovingData(
        Range{ event_info.Index(), event_info.Count() });

    Parts().SelectionStore().AdjustSelectionByRemovingIndexes(
        event_info.Index(),
        event_info.Count());
}


void ListCore::OnDataUpdated(const ListDataUpdatedInfo& event_info) {

    Parts().ItemHeightManager().OnDataUpdated(event_info);

    if (is_handling_data_source_event_) {
        refresh_after_data_source_event_ = true;
        return;
    }

    auto auto_reset = MakeAutoReset(is_handling_data_source_event_, true);
    HandleDataUpdated(event_info);
    RefreshItemsIfNeeded();
}


void ListCore::HandleDataUpdated(const ListDataUpdatedInfo& event_info) {

    auto item_container = item_container_.lock();
    if (!item_container) {
        return;
    }

    auto update_guard = item_container->BeginUpdate();

    float position_difference = AdjustContentHeight();

    Parts().VisibleItemManager().HandleDataUpdated(
        Range{ event_info.Index(), event_info.Count() }, 
        position_difference);
}


void ListCore::OnDataMoved(const ListDataMovedInfo& event_info) {

    Parts().ItemHeightManager().OnDataMoved(event_info);

    if (is_handling_data_source_event_) {
        refresh_after_data_source_event_ = true;
        return;
    }

    auto auto_reset = MakeAutoReset(is_handling_data_source_event_, true);
    HandleDataMoved(event_info);
    RefreshItemsIfNeeded();
}


void ListCore::HandleDataMoved(const ListDataMovedInfo& event_info) {

    auto item_container = item_container_.lock();
    if (!item_container) {
        return;
    }

    auto update_guard = item_container->BeginUpdate();

    AdjustContentHeight();

    Parts().VisibleItemManager().HandleDataMoved(
        event_info.PreviousIndex(), 
        event_info.NewIndex());

    Parts().FocusManager().ChangeFocusByMovingData(
        event_info.PreviousIndex(), 
        event_info.NewIndex());

    Parts().SelectionStore().AdjustSelectionByMovingIndex(
        event_info.PreviousIndex(),
        event_info.NewIndex());
}


void ListCore::RefreshItemsIfNeeded() {

    if (!refresh_after_data_source_event_) {
        return;
    }

    refresh_after_data_source_event_ = false;
    InnerReload(true);
}


float ListCore::AdjustContentHeight() {

    float old_total_height = current_total_height_;
    float new_total_height = Parts().ItemHeightManager().GetTotalHeight();

    if (old_total_height != new_total_height) {

        //Disable OnLayout() for preventing from reentering.
        auto auto_reset = MakeAutoReset(disable_on_layout_, true);
        SetScrollContentHeight(new_total_height);
        return new_total_height - old_total_height;
    }
    else {
        return 0;
    }
}


void ListCore::OnSelectionStoreChanged(const ListSelectionStoreChangedInfo& event_info) {

    Parts().VisibleItemManager().ChangeVisibleItemSelection(
        event_info.reason,
        event_info.changed_range);

    exit_selecting_by_mouse_sub_ = 
        Parts().InputHandler().WhenNotSelectingByMouse().Subscribe([this](None) {
    
        selection_changed_event_.AsObserver().OnNext({});
    });
}


void ListCore::SetScrollContentHeight(float height) {

    current_total_height_ = height;

    auto item_container = item_container_.lock();
    if (item_container) {
        item_container->SetFixedHeight(height);
    }
}


void ListCore::ScrollToItemAtIndex(std::size_t index) {

    auto position_and_height = Parts().ItemHeightManager().GetItemPositionAndHeight(index);

    Rect visible_scroll_area_rect = Parts().Owner().GetVisibleScrollContentRect();
    if (position_and_height.first < visible_scroll_area_rect.position.y) {
        Parts().Owner().ScrollToScrollContentPosition(Point(0, position_and_height.first));
    }
    else {

        float end_position = position_and_height.first + position_and_height.second;
        if (end_position > visible_scroll_area_rect.position.y + visible_scroll_area_rect.size.height) {

            Point scroll_to_position;
            scroll_to_position.x = 0;
            scroll_to_position.y = end_position - visible_scroll_area_rect.size.height;
            Parts().Owner().ScrollToScrollContentPosition(scroll_to_position);
        }
    }
}


std::optional<std::size_t> ListCore::FindItemIndexAtPosition(
    const Point& position) {

    auto visible_scroll_content_rect = Parts().Owner().GetVisibleScrollContentRect();

    if (position.x < 0 || position.x > visible_scroll_content_rect.size.width) {
        return std::nullopt;
    }

    float adjusted_position = position.y + visible_scroll_content_rect.position.y;
    return Parts().ItemHeightManager().GetItemIndex(adjusted_position);
}


rx::Observable<None> ListCore::SelectionChangedEvent() const {
    return selection_changed_event_.AsObservable();
}


rx::Observable<ListCoreItemDoubleClickInfo> ListCore::ItemDoubleClickEvent() const {
    return item_double_click_event_.AsObservable();
}

}