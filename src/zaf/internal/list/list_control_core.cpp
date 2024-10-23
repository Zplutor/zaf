#include <zaf/internal/list/list_control_core.h>
#include <zaf/base/as.h>
#include <zaf/base/auto_reset.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/range.h>
#include <zaf/internal/list/list_control_part_context.h>

namespace zaf::internal {

ListControlCore::ListControlCore(zaf::ScrollBox& owner) :
    part_context_(std::make_unique<ListControlPartContext>(this)),
    owner_(owner) {

}


ListControlCore::~ListControlCore() {
    UnregisterScrollBarEvents();
}


void ListControlCore::Initialize(const InitializeParameters& parameters) {

    owner_.SetCanFocused(true);
    owner_.SetBackgroundColor(Color::White());
    owner_.SetBorder(Frame(1));
    owner_.SetBorderColor(Color::Black());

    data_source_change_event_ = parameters.data_source_change_event;
    delegate_change_event_ = parameters.delegate_change_event;
    item_container_change_event_ = parameters.item_container_change_event;
    selection_changed_event_ = parameters.selection_changed_event;
    item_double_click_event_ = parameters.item_double_click_event;
    context_menu_event_ = parameters.context_menu_event;

    //Item container must be the first.
    InstallItemContainer(parameters.item_container);
    InstallDataSource(parameters.data_source);
    InstallDelegate(parameters.delegate);

    RegisterScrollBarEvents();

    Subscriptions() += part_context_->SelectionStore().ChangedEvent().Subscribe(
        std::bind_front(&ListControlCore::OnSelectionStoreChanged, this));

    Reload();
}


void ListControlCore::RegisterScrollBarEvents() {

    auto vertical_scroll_bar = owner_.VerticalScrollBar();

    vertical_scroll_bar_sub_ = vertical_scroll_bar->ScrollEvent().Subscribe(std::bind([this]() {
        part_context_->VisibleItemManager().UpdateVisibleItems();
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

    auto vertical_scroll_bar = owner_.VerticalScrollBar();
    vertical_scroll_bar->SetSmallChange(static_cast<int>(item_height));
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


void ListControlCore::InstallDataSource(
    const std::weak_ptr<ListDataSource>& data_source) {

    data_source_ = data_source;

    part_context_->ItemHeightManager().ResetDataSource(data_source);

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


void ListControlCore::SetDelegate(const std::weak_ptr<ListControlDelegate>& delegate) {

    auto previous_delegate = delegate_;
    
    InstallDelegate(delegate);

    if (delegate_change_event_) {
        delegate_change_event_(previous_delegate.lock());
    }

    Reload();
}


void ListControlCore::InstallDelegate(
    const std::weak_ptr<ListControlDelegate>& delegate) {

    delegate_ = delegate;
    part_context_->ItemHeightManager().ResetDelegate(delegate_);
}


void ListControlCore::SetItemContainer(
    const std::shared_ptr<ListItemContainer>& item_container) {

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

    item_container_subs_ += item_container_->MouseUpEvent().Subscribe(
        std::bind_front(&ListControlCore::OnItemContainerMouseUp, this));

    item_container_subs_ += item_container_->FocusGainedEvent().Subscribe(
        std::bind_front(&ListControlCore::OnItemContainerGainedFocus, this));

    item_container_subs_ += item_container_->FocusLostEvent().Subscribe(
        std::bind_front(&ListControlCore::OnItemContainerLostFocus, this));

    owner_.SetScrollContent(item_container_);
}


void ListControlCore::OnItemContainerDoubleClick(const DoubleClickInfo& event_info) {

    if (!item_double_click_event_) {
        return;
    }

    auto index = part_context_->ItemHeightManager().GetItemIndex(event_info.Position().y);
    if (index) {
        item_double_click_event_(*index);
    }
}


void ListControlCore::OnItemContainerMouseUp(const MouseUpInfo& event_info) {

    //Only handle right button event for now.
    if (event_info.Message().MouseButton() != MouseButton::Right) {
        return;
    }

    if (!context_menu_event_) {
        return;
    }

    auto item_index = part_context_->ItemHeightManager().GetItemIndex(event_info.PositionAtSender().y);

    std::shared_ptr<Object> item_data;
    if (item_index) {
        auto data_source = data_source_.lock();
        if (!data_source) {
            return;
        }
        item_data = data_source->GetDataAtIndex(*item_index);
    }

    auto menu = context_menu_event_(item_index, item_data);
    if (!menu) {
        return;
    }

    menu->PopupOnControl(item_container_, event_info.PositionAtSender());
    event_info.MarkAsHandled();
}


void ListControlCore::OnItemContainerGainedFocus(const FocusGainedInfo& event_info) {

    auto focused_control = As<Control>(event_info.Source());
    if (!focused_control) {
        return;
    }

    if (focused_control == item_container_) {
        return;
    }

    for (auto control = focused_control; control; control = control->Parent()) {

        auto list_item = As<ListItem>(control);
        if (list_item) {
            last_focused_item_data_ = list_item->ItemData();
        }
    }
}


void ListControlCore::OnItemContainerLostFocus(const FocusLostInfo& event_info) {

    //Focus was taken from other control outside list control, clear last_focused_item_data_.
    auto last_focused_item_data = last_focused_item_data_.lock();
    if (last_focused_item_data == event_info.Source()) {

        if (event_info.GainedFocusControl()) {
            last_focused_item_data_.reset();
            return;
        }
    }
}


void ListControlCore::OnLayout() {

    if (!disable_on_layout_) {
        part_context_->VisibleItemManager().UpdateVisibleItems();
    }
}


void ListControlCore::OnVerticalScrollBarChange(
    const std::shared_ptr<ScrollBar>& previous_scroll_bar) {

    UnregisterScrollBarEvents();
    RegisterScrollBarEvents();
    AdjustScrollBarSmallChange();
}


void ListControlCore::Reload() {
    InnerReload(false);
}


void ListControlCore::InnerReload(bool retain_state) {

    if (!retain_state) {
        //Remove selected indexes.
        part_context_->SelectionManager().UnselectAllItems();
    }

    //Remove all visible items.
    auto& visible_item_manager = part_context_->VisibleItemManager();
    visible_item_manager.ClearVisibleItems();

    UpdateContentHeight();
    visible_item_manager.UpdateVisibleItems();
    AdjustScrollBarSmallChange();
}


void ListControlCore::UpdateContentHeight() {

    part_context_->ItemHeightManager().ReloadItemHeights();
    SetScrollContentHeight(part_context_->ItemHeightManager().GetTotalHeight());
}


void ListControlCore::RecoverLastFocusedItem(
    const std::vector<std::shared_ptr<ListItem>>& items) {

    auto last_focused_item_data = last_focused_item_data_.lock();
    if (!last_focused_item_data) {
        return;
    }

    for (const auto& each_item : items) {

        if (each_item->ItemData() == last_focused_item_data) {
            each_item->RecoverFocus();
            break;
        }
    }
}

void ListControlCore::OnDataAdded(const ListDataAddedInfo& event_info) {

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
        !part_context_->VisibleItemManager().HasVisibleItem();

    auto update_guard = item_container_->BeginUpdate();

    float position_difference = AdjustContentHeight();

    part_context_->VisibleItemManager().HandleDataAdded(
        Range{ event_info.Index(), event_info.Count() },
        position_difference);

    part_context_->SelectionStore().AdjustSelectionByAddingIndexes(
        event_info.Index(),
        event_info.Count());

    if (need_adjust_scroll_bar_small_change) {
        AdjustScrollBarSmallChange();
    }
}


void ListControlCore::OnDataRemoved(const ListDataRemovedInfo& event_info) {

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

    part_context_->VisibleItemManager().HandleDataRemoved(
        Range{ event_info.Index(), event_info.Count() },
        position_difference);

    part_context_->SelectionStore().AdjustSelectionByRemovingIndexes(
        event_info.Index(),
        event_info.Count());
}


void ListControlCore::OnDataUpdated(const ListDataUpdatedInfo& event_info) {

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

    part_context_->VisibleItemManager().HandleDataUpdated(
        Range{ event_info.Index(), event_info.Count() }, 
        position_difference);
}


void ListControlCore::OnDataMoved(const ListDataMovedInfo& event_info) {

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

    part_context_->VisibleItemManager().HandleDataMoved(
        event_info.PreviousIndex(), 
        event_info.NewIndex());

    part_context_->SelectionStore().AdjustSelectionByMovingIndex(
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
    float new_total_height = part_context_->ItemHeightManager().GetTotalHeight();

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

    part_context_->VisibleItemManager().ChangeVisibleItemSelection(
        event_info.reason,
        event_info.changed_range);

    exit_handle_mouse_event_sub_ = 
        part_context_->InputHandler().WhenNotHandlingMouseEvent().Subscribe([this](None) {
    
        if (selection_changed_event_) {
            selection_changed_event_();
        }
    });
}


void ListControlCore::SetScrollContentHeight(float height) {

    current_total_height_ = height;
    item_container_->SetFixedHeight(height);
}


std::size_t ListControlCore::GetItemCount() {
    return part_context_->ItemHeightManager().GetItemCount();
}


void ListControlCore::ScrollToItemAtIndex(std::size_t index) {

    auto position_and_height = part_context_->ItemHeightManager().GetItemPositionAndHeight(index);

    Rect visible_scroll_area_rect = owner_.GetVisibleScrollContentRect();
    if (position_and_height.first < visible_scroll_area_rect.position.y) {
        owner_.ScrollToScrollContentPosition(Point(0, position_and_height.first));
    }
    else {

        float end_position = position_and_height.first + position_and_height.second;
        if (end_position > visible_scroll_area_rect.position.y + visible_scroll_area_rect.size.height) {

            Point scroll_to_position;
            scroll_to_position.x = 0;
            scroll_to_position.y = end_position - visible_scroll_area_rect.size.height;
            owner_.ScrollToScrollContentPosition(scroll_to_position);
        }
    }
}


std::optional<std::size_t> ListControlCore::FindItemIndexAtPosition(
    const Point& position) {

    auto visible_scroll_content_rect = owner_.GetVisibleScrollContentRect();

    if (position.x < 0 || position.x > visible_scroll_content_rect.size.width) {
        return std::nullopt;
    }

    float adjusted_position = position.y + visible_scroll_content_rect.position.y;
    return part_context_->ItemHeightManager().GetItemIndex(adjusted_position);
}

}