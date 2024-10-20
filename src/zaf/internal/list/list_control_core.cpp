#include <zaf/internal/list/list_control_core.h>
#include <zaf/base/as.h>
#include <zaf/base/auto_reset.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/range.h>
#include <zaf/internal/list/list_control_part_context.h>

namespace zaf::internal {
namespace {

void CalculateRangeDifference(
    std::size_t new_index,
    std::size_t new_count,
    std::size_t old_index,
    std::size_t old_count,
    bool& remove_head,
    std::size_t& head_change_count,
    bool& remove_tail,
    std::size_t& tail_change_count) {

    if (old_count == 0) {
        remove_head = false;
        head_change_count = 0;
        remove_tail = false;
        tail_change_count = new_count;
        return;
    }

    std::size_t new_end_index = new_index + new_count;
    std::size_t old_end_index = old_index + old_count;

    //Exclude
    if ((new_end_index <= old_index) || (new_index >= old_end_index)) {

        remove_head = true;
        head_change_count = old_count;

        remove_tail = false;
        tail_change_count = new_count;
    }
    //Intersect
    else {

        if (new_index > old_index) {
            remove_head = true;
            head_change_count = new_index - old_index;
        }
        else {
            remove_head = false;
            head_change_count = old_index - new_index;
        }

        if (new_end_index > old_end_index) {
            remove_tail = false;
            tail_change_count = new_end_index - old_end_index;
        }
        else {
            remove_tail = true;
            tail_change_count = old_end_index - new_end_index;
        }
    }
}

}

ListControlCore::ListControlCore(ScrollBox& owner) :
    part_context_(std::make_unique<ListControlPartContext>(this)),
    owner_(owner) {

}


ListControlCore::~ListControlCore() {

    UnregisterScrollBarEvents(owner_.VerticalScrollBar());
}


void ListControlCore::Initialize(const InitializeParameters& parameters) {

    owner_.SetCanFocused(true);
    owner_.SetBackgroundColor(Color::White());
    owner_.SetBorder(Frame(1));
    owner_.SetBorderColor(Color::Black());

    data_source_change_event_ = parameters.data_source_change_event;
    delegate_change_event_ = parameters.delegate_change_event;
    item_container_change_event_ = parameters.item_container_change_event;
    selection_change_event_ = parameters.selection_change_event;
    item_double_click_event_ = parameters.item_double_click_event;
    context_menu_event_ = parameters.context_menu_event;

    //Item container must be the first.
    InstallItemContainer(parameters.item_container);
    InstallDataSource(parameters.data_source);
    InstallDelegate(parameters.delegate);

    //Temporary solution.
    Subscriptions() += part_context_->SelectionManager().SelectionChangedEvent().Subscribe(
        [this](const ListSelectionChangedInfo& event_info) {
            if (selection_change_event_) {
                selection_change_event_(event_info.reason, event_info.index, event_info.count);
            }
        });

    SetSelectionMode(SelectionMode::Single);
    RegisterScrollBarEvents();

    Reload();
}


void ListControlCore::RegisterScrollBarEvents() {

    auto vertical_scroll_bar = owner_.VerticalScrollBar();

    vertical_scroll_bar_sub_ = vertical_scroll_bar->ScrollEvent().Subscribe(
        std::bind(&ListControlCore::UpdateVisibleItems, this));
}


void ListControlCore::UnregisterScrollBarEvents(
    const std::shared_ptr<ScrollBar>& scroll_bar) {

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


SelectionMode ListControlCore::GetSelectionMode() const noexcept {
    return part_context_->SelectionManager().SelectionMode();
}


void ListControlCore::SetSelectionMode(SelectionMode mode) {

    part_context_->SelectionManager().SetSelectionMode(mode);

    if (mode == SelectionMode::Single) {

        auto first_selected_index = GetFirstSelectedItemIndex();
        if (first_selected_index) {

            ReplaceSelection(*first_selected_index, 1);
            NotifySelectionChange(
                ListSelectionChangeReason::ReplaceSelection,
                *first_selected_index, 
                1);
        }
    }
    else if (mode == SelectionMode::None) {
        UnselectAllItems();
    }
}


void ListControlCore::OnLayout() {

    if (!disable_on_layout_) {
        UpdateVisibleItems();
    }
}


void ListControlCore::OnVerticalScrollBarChange(
    const std::shared_ptr<ScrollBar>& previous_scroll_bar) {

    if (previous_scroll_bar) {
        UnregisterScrollBarEvents(previous_scroll_bar);
    }

    RegisterScrollBarEvents();
    AdjustScrollBarSmallChange();
}


void ListControlCore::Reload() {
    InnerReload(false);
}


void ListControlCore::InnerReload(bool retain_state) {

    if (!retain_state) {
        //Remove selected indexes.
        UnselectAllItems();
    }

    //Remove all visible items.
    if (!visible_items_.empty()) {

        auto update_guard = item_container_->BeginUpdate();
        for (const auto& each_item : visible_items_) {
            item_container_->RemoveChild(each_item);
        }

        visible_items_.clear();

        if (!retain_state) {
            first_visible_item_index_ = 0;
            last_focused_item_data_.reset();
        }
    }

    UpdateContentHeight();
    UpdateVisibleItems();
    AdjustScrollBarSmallChange();
}


void ListControlCore::UpdateContentHeight() {

    part_context_->ItemHeightManager().ReloadItemHeights();
    SetScrollContentHeight(part_context_->ItemHeightManager().GetTotalHeight());
}


void ListControlCore::UpdateVisibleItems() {

    std::size_t old_index = first_visible_item_index_;
    std::size_t old_count = visible_items_.size();

    //Get new visible items range.
    std::size_t new_index = 0;
    std::size_t new_count = 0;
    GetVisibleItemsRange(new_index, new_count);

    //Calculate the difference.
    bool remove_head = false;
    std::size_t head_change_count = 0;
    bool remove_tail = false;
    std::size_t tail_change_count = 0;

    CalculateRangeDifference(
        new_index,
        new_count,
        old_index,
        old_count,
        remove_head,
        head_change_count,
        remove_tail,
        tail_change_count);

    if ((head_change_count == 0) && (tail_change_count == 0)) {
        return;
    }

    AdjustVisibleItems(
        new_index,
        new_count,
        remove_head,
        head_change_count,
        remove_tail,
        tail_change_count);

    first_visible_item_index_ = new_index;
}


void ListControlCore::GetVisibleItemsRange(std::size_t& index, std::size_t& count) {

    auto visible_rect = owner_.GetVisibleScrollContentRect();
    float begin_position = visible_rect.position.y;
    float end_position = begin_position + visible_rect.size.height;

    //No need further calculation if the rect is abnormal.
    if (end_position <= begin_position) {
        index = 0;
        count = 0;
        return;
    }

    auto index_and_count = part_context_->ItemHeightManager().GetItemRange(begin_position, end_position);
    index = index_and_count.first;
    count = index_and_count.second;

    if (index > 2) {
        index -= 2;
    }
    else {
        index = 0;
    }

    count += 2;
    std::size_t max_count = GetItemCount() - index;
    if (count > max_count) {
        count = max_count;
    }
}


void ListControlCore::AdjustVisibleItems(
    std::size_t new_index,
    std::size_t new_count,
    bool remove_head,
    std::size_t head_change_count,
    bool remove_tail,
    std::size_t tail_change_count) {

    auto update_guard = item_container_->BeginUpdate();

    if (remove_head) {
        RemoveHeadVisibleItems(head_change_count);
    }
    else {
        if (head_change_count > 0) {
            auto new_head_items = CreateItems(new_index, head_change_count);
            visible_items_.insert(visible_items_.begin(), new_head_items.begin(), new_head_items.end());
            RecoverLastFocusedItem(new_head_items);
        }
    }

    if (remove_tail) {
        RemoveTailVisibleItems(tail_change_count);
    }
    else {
        if (tail_change_count > 0) {
            auto new_tail_items = CreateItems(new_index + new_count - tail_change_count, tail_change_count);
            visible_items_.insert(visible_items_.end(), new_tail_items.begin(), new_tail_items.end());
            RecoverLastFocusedItem(new_tail_items);
        }
    }
}


void ListControlCore::RemoveHeadVisibleItems(std::size_t count) {

    for (std::size_t current_count = 0; current_count < count; ++current_count) {
        auto item = visible_items_.front();
        visible_items_.pop_front();
        item_container_->RemoveChild(item);
    }
}


void ListControlCore::RemoveTailVisibleItems(std::size_t count) {

    for (std::size_t current_count = 0; current_count < count; ++current_count) {
        auto item = visible_items_.back();
        visible_items_.pop_back();
        item_container_->RemoveChild(item);
    }
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


std::vector<std::shared_ptr<ListItem>> ListControlCore::CreateItems(
    std::size_t index,
    std::size_t count) {

    std::vector<std::shared_ptr<ListItem>> items;
    items.reserve(count);

    for (std::size_t current_index = index; current_index < index + count; ++current_index) {

        auto new_item = CreateItem(current_index);
        if (!new_item) {
            continue;
        }

        item_container_->AddChild(new_item);
        items.push_back(new_item);
    }

    return items;
}


std::shared_ptr<ListItem> ListControlCore::CreateItem(std::size_t index) {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return nullptr;
    }

    auto delegate = delegate_.lock();
    if (!delegate) {
        return nullptr;
    }

    auto item_data = data_source->GetDataAtIndex(index);
    auto list_item = delegate->CreateItem(index, item_data);
    list_item->SetItemData(item_data);

    auto item_text = delegate->GetItemText(index, item_data);
    if (!item_text.empty()) {
        list_item->SetText(item_text);
    }
    
    delegate->LoadItem(list_item, index);

    auto position_and_height = part_context_->ItemHeightManager().GetItemPositionAndHeight(index);
    Rect item_rect;
    item_rect.position.y = position_and_height.first;
    item_rect.size.height = position_and_height.second;
    list_item->SetRect(item_rect);

    list_item->SetIsSelected(part_context_->SelectionStore().IsIndexSelected(index));
    return list_item;
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
    bool need_adjust_scroll_bar_small_change = visible_items_.empty();

    bool selection_changed = part_context_->SelectionStore().AdjustSelectionByAddingIndexes(
        event_info.Index(),
        event_info.Count());

    auto update_guard = item_container_->BeginUpdate();

    float position_difference = AdjustContentHeight();

    auto first_need_update_index = AddVisibleItems(event_info.Index(), event_info.Count());
    if (first_need_update_index) {
        AdjustVisibleItemPositions(*first_need_update_index, position_difference);
    }
    UpdateVisibleItems();

    if (selection_changed) {
        NotifySelectionChange(ListSelectionChangeReason::ItemChange, 0, 0);
    }

    if (need_adjust_scroll_bar_small_change) {
        AdjustScrollBarSmallChange();
    }
}


//Returns the index of the first visible item which needs to be adjusted its position.
std::optional<std::size_t> ListControlCore::AddVisibleItems(
    std::size_t index,
    std::size_t count) {

    if (index >= first_visible_item_index_ + visible_items_.size()) {
        return std::nullopt;
    }

    if (index <= first_visible_item_index_) {
        first_visible_item_index_ += count;
        return 0;
    }

    return AddMiddleVisibleItems(index, count);
}


std::optional<std::size_t> ListControlCore::AddMiddleVisibleItems(
    std::size_t index,
    std::size_t count) {

    std::size_t insert_index = index - first_visible_item_index_;
    std::size_t need_adjust_position_count = visible_items_.size() - insert_index;

    if (count >= need_adjust_position_count) {
        RemoveTailVisibleItems(need_adjust_position_count);
        return std::nullopt;
    }
    else {

        auto new_items = CreateItems(index, count);
        visible_items_.insert(
            std::next(visible_items_.begin(), insert_index),
            new_items.begin(), 
            new_items.end());

        return insert_index + new_items.size();
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

    bool selection_changed = part_context_->SelectionStore().AdjustSelectionByRemovingIndexes(
        event_info.Index(),
        event_info.Count());

    auto update_guard = item_container_->BeginUpdate();

    float position_difference = AdjustContentHeight();

    auto first_need_update_index = RemoveVisibleItems(event_info.Index(), event_info.Count());
    if (first_need_update_index) {
        AdjustVisibleItemPositions(*first_need_update_index, position_difference);
        UpdateVisibleItems();
    }

    if (selection_changed) {
        NotifySelectionChange(ListSelectionChangeReason::ItemChange, 0, 0);
    }
}


//Returns the index of the first visible item which needs to be adjusted its position.
std::optional<std::size_t> ListControlCore::RemoveVisibleItems(
    std::size_t index, 
    std::size_t count) {
    
    if (index >= first_visible_item_index_ + visible_items_.size()) {
        return std::nullopt;
    }

    if (index < first_visible_item_index_) {
        first_visible_item_index_ -= count;
        return 0;
    }

    return RemoveMiddleVisibleItems(index, count);
}


std::size_t ListControlCore::RemoveMiddleVisibleItems(
    std::size_t index,
    std::size_t count) {

    std::size_t remove_index = index - first_visible_item_index_;
    std::size_t remove_count = (std::min)(count, visible_items_.size() - remove_index);

    auto begin_erase_iterator = std::next(visible_items_.begin(), remove_index);
    auto end_erase_iterator = std::next(begin_erase_iterator, remove_count);

    for (auto iterator = begin_erase_iterator; iterator != end_erase_iterator; ++iterator) {
        item_container_->RemoveChild(*iterator);
    }

    visible_items_.erase(begin_erase_iterator, end_erase_iterator);
    return remove_index;
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

    if (event_info.Index() >= first_visible_item_index_ + visible_items_.size()) {
        return;
    }

    AdjustVisibleItemPositionsByUpdatingItems(
        event_info.Index(),
        event_info.Count(),
        position_difference);

    UpdateVisibleItemsByUpdatingItems(event_info.Index(), event_info.Count());
}


void ListControlCore::AdjustVisibleItemPositionsByUpdatingItems(
    std::size_t index,
    std::size_t count,
    float position_difference) {

    if (position_difference == 0) {
        return;
    }

    std::size_t end_update_index = index + count;
    std::size_t begin_adjust_index{};

    if (end_update_index <= first_visible_item_index_) {
        begin_adjust_index = 0;
    }
    else if (end_update_index < first_visible_item_index_ + visible_items_.size()) {
        begin_adjust_index = end_update_index - first_visible_item_index_;
    }

    AdjustVisibleItemPositions(begin_adjust_index, position_difference);
    UpdateVisibleItems();
}


void ListControlCore::UpdateVisibleItemsByUpdatingItems(
    std::size_t index, 
    std::size_t count) {

    std::size_t end_update_index = index + count;
    std::size_t end_visible_item_index = first_visible_item_index_ + visible_items_.size();

    if ((index >= end_visible_item_index) || (first_visible_item_index_ >= end_update_index)) {
        return;
    }

    std::size_t intersect_begin_update_index = (std::max)(index, first_visible_item_index_);
    std::size_t intersect_end_update_index = (std::min)(end_update_index, end_visible_item_index);

    std::vector<std::shared_ptr<ListItem>> new_items;

    for (std::size_t current_index = intersect_begin_update_index; 
         current_index < intersect_end_update_index; 
         ++current_index) {

        std::size_t visible_item_index = current_index - first_visible_item_index_;

        auto current_item = visible_items_[visible_item_index];
        item_container_->RemoveChild(current_item);

        auto new_item = CreateItem(current_index);
        item_container_->AddChild(new_item);
        visible_items_[visible_item_index] = new_item;

        new_items.push_back(new_item);
    }

    RecoverLastFocusedItem(new_items);
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

    bool has_selection = !!part_context_->SelectionStore().GetFirstSelectedIndex();

    part_context_->SelectionStore().AdjustSelectionByMovingIndex(
        event_info.PreviousIndex(),
        event_info.NewIndex());

    auto update_guard = item_container_->BeginUpdate();

    float position_difference = AdjustContentHeight();

    RemoveVisibleItems(event_info.PreviousIndex(), 1);
    AddVisibleItems(event_info.NewIndex(), 1);

    for (auto visible_index : Range{ 0, visible_items_.size() }) {

        auto item_index = visible_index + first_visible_item_index_;
        auto [position, height] = part_context_->ItemHeightManager().GetItemPositionAndHeight(item_index);
        visible_items_[visible_index]->SetY(position);
    }

    UpdateVisibleItems();

    if (has_selection) {
        NotifySelectionChange(ListSelectionChangeReason::ItemChange, 0, 0);
    }
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


void ListControlCore::SetScrollContentHeight(float height) {

    current_total_height_ = height;
    item_container_->SetFixedHeight(height);
}


void ListControlCore::AdjustVisibleItemPositions(
    std::size_t begin_adjust_index, 
    float difference) {

    for (std::size_t index = begin_adjust_index; index < visible_items_.size(); ++index) {

        const auto& item = visible_items_[index];
        auto rect = item->Rect();
        rect.position.y += difference;
        item->SetRect(rect);
    }
}


void ListControlCore::UnselectAllItems() {
    part_context_->SelectionManager().UnselectAllItems();
}


void ListControlCore::SelectItemAtIndex(std::size_t index) {
    part_context_->SelectionManager().SelectItemAtIndex(index);
}


void ListControlCore::UnselectItemAtIndex(std::size_t index) {
    part_context_->SelectionManager().UnselectItemAtIndex(index);
}


std::size_t ListControlCore::GetItemCount() {
    return part_context_->ItemHeightManager().GetItemCount();
}


std::shared_ptr<ListItem> ListControlCore::GetVisibleItemAtIndex(
    std::size_t index) const noexcept {

    if (index < first_visible_item_index_ || 
        index >= first_visible_item_index_ + visible_items_.size()) {
        return nullptr;
    }

    auto index_in_visible_items = index - first_visible_item_index_;
    return visible_items_[index_in_visible_items];
}


std::size_t ListControlCore::GetSelectedItemCount() {
    return part_context_->SelectionStore().GetAllSelectedCount();
}


std::optional<std::size_t> ListControlCore::GetFirstSelectedItemIndex() const noexcept {
    return part_context_->SelectionStore().GetFirstSelectedIndex();
}


std::vector<std::size_t> ListControlCore::GetAllSelectedItemIndexes() {
    return part_context_->SelectionStore().GetAllSelectedIndexes();
}


bool ListControlCore::IsItemSelectedAtIndex(std::size_t index) {
    return part_context_->SelectionStore().IsIndexSelected(index);
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


std::optional<std::size_t> ListControlCore::GetListItemIndex(
    const std::shared_ptr<ListItem>& item) {

    for (auto index : Range(0, visible_items_.size())) {

        if (visible_items_[index] == item) {
            return index + first_visible_item_index_;
        }
    }

    return std::nullopt;
}


void ListControlCore::ReplaceSelection(std::size_t index, std::size_t count) {

    part_context_->SelectionStore().ReplaceSelection(index, count);

    for (std::size_t visible_item_index = 0; 
         visible_item_index < visible_items_.size(); 
         ++visible_item_index) {

        const auto& each_visible_item = visible_items_[visible_item_index];

        std::size_t item_index = first_visible_item_index_ + visible_item_index;
        bool is_selected = (index <= item_index) && (item_index < index + count);
        each_visible_item->SetIsSelected(is_selected);
    }
}


bool ListControlCore::RevertSelection(std::size_t index) {

    bool is_selected = part_context_->SelectionStore().RevertSelection(index);

    if ((first_visible_item_index_ <= index) && (index < first_visible_item_index_ + visible_items_.size())) {

        const auto& visible_item = visible_items_[index - first_visible_item_index_];
        visible_item->SetIsSelected(!visible_item->IsSelected());
    }

    return is_selected;
}


void ListControlCore::AddSelection(std::size_t index, std::size_t count) {
    ChangeSelection(index, count, true);
}


void ListControlCore::RemoveSelection(std::size_t index, std::size_t count) {
    ChangeSelection(index, count, false);
}


void ListControlCore::ChangeSelection(std::size_t index, std::size_t count, bool is_add) {

    if (is_add) {
        part_context_->SelectionStore().AddSelection(index, count);
    }
    else {
        part_context_->SelectionStore().RemoveSelection(index, count);
    }

    std::size_t intersect_begin_index = (std::max)(index, first_visible_item_index_);
    std::size_t intersect_end_index = 
        (std::min)(index + count, first_visible_item_index_ + visible_items_.size());

    if (intersect_begin_index >= intersect_end_index) {
        return;
    }

    for (std::size_t visible_item_index = intersect_begin_index - first_visible_item_index_;
        visible_item_index < intersect_end_index - first_visible_item_index_;
        ++visible_item_index) {

        visible_items_[visible_item_index]->SetIsSelected(is_add);
    }
}


void ListControlCore::NotifySelectionChange(
    ListSelectionChangeReason change_type,
    std::size_t index,
    std::size_t count) {

    if (selection_change_event_) {
        selection_change_event_(change_type, index, count);
    }
}

}