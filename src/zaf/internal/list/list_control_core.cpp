#include <zaf/internal/list/list_control_core.h>
#include <zaf/base/as.h>
#include <zaf/base/auto_reset.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/range.h>
#include <zaf/internal/list/list_control_parts_context.h>

namespace zaf::internal {

void ListControlCore::Initialize(const InitializeParameters& parameters) {

    auto& owner = Parts().Owner();
    owner.SetCanFocused(true);
    owner.SetBackgroundColor(Color::White());
    owner.SetBorder(Frame(1));
    owner.SetBorderColor(Color::Black());

    data_source_change_event_ = parameters.data_source_change_event;
    delegate_change_event_ = parameters.delegate_change_event;
    item_container_change_event_ = parameters.item_container_change_event;
    selection_changed_event_ = parameters.selection_changed_event;
    item_double_click_event_ = parameters.item_double_click_event;

    //Item container must be the first.
    InstallItemContainer(parameters.item_container);
    InstallDataSource(parameters.data_source);
    InstallDelegate(parameters.delegate);

    RegisterScrollBarEvents();

    Subscriptions() += Parts().SelectionStore().ChangedEvent().Subscribe(
        std::bind_front(&ListControlCore::OnSelectionStoreChanged, this));

    Reload();
}


void ListControlCore::RegisterScrollBarEvents() {

    auto vertical_scroll_bar = Parts().Owner().VerticalScrollBar();

    vertical_scroll_bar_sub_ = vertical_scroll_bar->ScrollEvent().Subscribe(std::bind([this]() {
        Parts().VisibleItemManager().UpdateVisibleItems();
    }));
}


void ListControlCore::UnregisterScrollBarEvents() {
    vertical_scroll_bar_sub_.Unsubscribe();
}


void ListControlCore::AdjustScrollBarSmallChange() {

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


std::shared_ptr<ListDataSource> ListControlCore::DataSource() const noexcept {
    return data_source_.lock();
}


void ListControlCore::SetDataSource(const std::weak_ptr<ListDataSource>& data_source) {

    data_source_subs_.Clear();

    auto previous_data_source = data_source_;

    InstallDataSource(data_source);

    if (data_source_change_event_) {
        data_source_change_event_(previous_data_source.lock());
    }

    Reload();
}


void ListControlCore::InstallDataSource(const std::weak_ptr<ListDataSource>& data_source) {

    data_source_ = data_source;

    Parts().ItemHeightManager().ResetDataSource(data_source);

    RegisterDataSourceEvents();
}


void ListControlCore::RegisterDataSourceEvents() {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    data_source_subs_ += data_source->DataAddedEvent().Subscribe(
        std::bind_front(&ListControlCore::OnDataAdded, this));

    data_source_subs_ += data_source->DataRemovedEvent().Subscribe(
        std::bind_front(&ListControlCore::OnDataRemoved, this));

    data_source_subs_ += data_source->DataUpdatedEvent().Subscribe(
        std::bind_front(&ListControlCore::OnDataUpdated, this));

    data_source_subs_ += data_source->DataMovedEvent().Subscribe(
        std::bind_front(&ListControlCore::OnDataMoved, this));
}


std::shared_ptr<ListControlDelegate> ListControlCore::Delegate() const noexcept {
    return delegate_.lock();
}


void ListControlCore::SetDelegate(const std::weak_ptr<ListControlDelegate>& delegate) {

    auto previous_delegate = delegate_;
    
    InstallDelegate(delegate);

    if (delegate_change_event_) {
        delegate_change_event_(previous_delegate.lock());
    }

    Reload();
}


void ListControlCore::InstallDelegate(const std::weak_ptr<ListControlDelegate>& delegate) {

    delegate_ = delegate;
    Parts().ItemHeightManager().ResetDelegate(delegate_);
}


const std::shared_ptr<ListItemContainer>& ListControlCore::ItemContainer() const noexcept {
    return item_container_;
}


void ListControlCore::SetItemContainer(const std::shared_ptr<ListItemContainer>& item_container) {

    if (item_container_ == item_container) {
        return;
    }

    auto previous_item_container = item_container_;

    item_container_subs_.Clear();
    InstallItemContainer(item_container);

    if (item_container_change_event_) {
        item_container_change_event_(previous_item_container);
    }
    
    Reload();
}


void ListControlCore::InstallItemContainer(
    const std::shared_ptr<ListItemContainer>& item_container) {

    ZAF_EXPECT(item_container);

    item_container_ = item_container;

    item_container_subs_ += item_container_->DoubleClickEvent().Subscribe(
        std::bind_front(&ListControlCore::OnItemContainerDoubleClick, this));

    Parts().Owner().SetScrollContent(item_container_);
}


void ListControlCore::OnItemContainerDoubleClick(const DoubleClickInfo& event_info) {

    if (!item_double_click_event_) {
        return;
    }

    auto index = Parts().ItemHeightManager().GetItemIndex(event_info.Position().y);
    if (index) {
        item_double_click_event_(*index);
    }
}


void ListControlCore::OnLayout() {

    if (!disable_on_layout_) {
        Parts().VisibleItemManager().UpdateVisibleItems();
    }
}


void ListControlCore::OnVerticalScrollBarChange() {

    UnregisterScrollBarEvents();
    RegisterScrollBarEvents();
    AdjustScrollBarSmallChange();
}


void ListControlCore::HandleFocusGainedEvent(const FocusGainedInfo& event_info) {

    Parts().FocusManager().ChangeFocusByOwnerFocusGainedEvent(event_info);

    auto scroll_box = As<ScrollBox>(event_info.Source());
    if (scroll_box && scroll_box.get() == &Parts().Owner()) {
        RepaintSelectedItems();
    }
}


void ListControlCore::HandleFocusLostEvent(const FocusLostInfo& event_info) {

    Parts().FocusManager().ChangeFocusByOwnerFocusLostEvent(event_info);

    auto scroll_box = As<ScrollBox>(event_info.Source());
    if (scroll_box && scroll_box.get() == &Parts().Owner()) {
        RepaintSelectedItems();
    }
}


void ListControlCore::RepaintSelectedItems() {

    auto selected_indexes = Parts().SelectionStore().GetAllSelectedIndexes();

    auto& visible_item_manager = Parts().VisibleItemManager();
    for (auto each_index : selected_indexes) {

        auto visible_item = visible_item_manager.GetVisibleItemAtIndex(each_index);
        if (visible_item && visible_item->IsSelected()) {
            visible_item->NeedRepaint();
        }
    }
}


void ListControlCore::Reload() {
    InnerReload(false);
}


void ListControlCore::InnerReload(bool retain_state) {

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


void ListControlCore::UpdateContentHeight() {

    Parts().ItemHeightManager().ReloadItemHeights();
    SetScrollContentHeight(Parts().ItemHeightManager().GetTotalHeight());
}


void ListControlCore::OnDataAdded(const ListDataAddedInfo& event_info) {

    Parts().ItemHeightManager().OnDataAdded(event_info);

    if (is_handling_data_source_event_) {
        refresh_after_data_source_event_ = true;
        return;
    }

    auto auto_reset = MakeAutoReset(is_handling_data_source_event_, true);
    HandleDataAdded(event_info);
    RefreshItemsIfNeeded();
}


void ListControlCore::HandleDataAdded(const ListDataAddedInfo& event_info) {

    //Adjust scroll bar small change if there is no items before adding.
    bool need_adjust_scroll_bar_small_change = 
        !Parts().VisibleItemManager().HasVisibleItem();

    auto update_guard = item_container_->BeginUpdate();

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


void ListControlCore::OnDataRemoved(const ListDataRemovedInfo& event_info) {

    Parts().ItemHeightManager().OnDataRemoved(event_info);

    if (is_handling_data_source_event_) {
        refresh_after_data_source_event_ = true;
        return;
    }

    auto auto_reset = MakeAutoReset(is_handling_data_source_event_, true);
    HandleDataRemoved(event_info);
    RefreshItemsIfNeeded();
}


void ListControlCore::HandleDataRemoved(const ListDataRemovedInfo& event_info) {

    auto update_guard = item_container_->BeginUpdate();

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


void ListControlCore::OnDataUpdated(const ListDataUpdatedInfo& event_info) {

    Parts().ItemHeightManager().OnDataUpdated(event_info);

    if (is_handling_data_source_event_) {
        refresh_after_data_source_event_ = true;
        return;
    }

    auto auto_reset = MakeAutoReset(is_handling_data_source_event_, true);
    HandleDataUpdated(event_info);
    RefreshItemsIfNeeded();
}


void ListControlCore::HandleDataUpdated(const ListDataUpdatedInfo& event_info) {

    auto update_guard = item_container_->BeginUpdate();

    float position_difference = AdjustContentHeight();

    Parts().VisibleItemManager().HandleDataUpdated(
        Range{ event_info.Index(), event_info.Count() }, 
        position_difference);
}


void ListControlCore::OnDataMoved(const ListDataMovedInfo& event_info) {

    Parts().ItemHeightManager().OnDataMoved(event_info);

    if (is_handling_data_source_event_) {
        refresh_after_data_source_event_ = true;
        return;
    }

    auto auto_reset = MakeAutoReset(is_handling_data_source_event_, true);
    HandleDataMoved(event_info);
    RefreshItemsIfNeeded();
}


void ListControlCore::HandleDataMoved(const ListDataMovedInfo& event_info) {

    auto update_guard = item_container_->BeginUpdate();

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


void ListControlCore::RefreshItemsIfNeeded() {

    if (!refresh_after_data_source_event_) {
        return;
    }

    refresh_after_data_source_event_ = false;
    InnerReload(true);
}


float ListControlCore::AdjustContentHeight() {

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


void ListControlCore::OnSelectionStoreChanged(const ListSelectionStoreChangedInfo& event_info) {

    Parts().VisibleItemManager().ChangeVisibleItemSelection(
        event_info.reason,
        event_info.changed_range);

    exit_selecting_by_mouse_sub_ = 
        Parts().InputHandler().WhenNotSelectingByMouse().Subscribe([this](None) {
    
        if (selection_changed_event_) {
            selection_changed_event_();
        }
    });
}


void ListControlCore::SetScrollContentHeight(float height) {

    current_total_height_ = height;
    item_container_->SetFixedHeight(height);
}


void ListControlCore::ScrollToItemAtIndex(std::size_t index) {

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


std::optional<std::size_t> ListControlCore::FindItemIndexAtPosition(
    const Point& position) {

    auto visible_scroll_content_rect = Parts().Owner().GetVisibleScrollContentRect();

    if (position.x < 0 || position.x > visible_scroll_content_rect.size.width) {
        return std::nullopt;
    }

    float adjusted_position = position.y + visible_scroll_content_rect.position.y;
    return Parts().ItemHeightManager().GetItemIndex(adjusted_position);
}

}