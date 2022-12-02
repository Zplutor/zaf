#include <zaf/control/internal/list_control/list_control_implementation.h>
#include <zaf/base/container/utility/range.h>
#include <zaf/base/error/check.h>
#include <zaf/control/internal/list_control/list_control_extended_multiple_select_strategy.h>
#include <zaf/control/internal/list_control/list_control_item_height_manager.h>
#include <zaf/control/internal/list_control/list_control_simple_multiple_select_strategy.h>
#include <zaf/control/internal/list_control/list_control_single_select_strategy.h>

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

ListControlImplementation::ListControlImplementation(ScrollableControl& owner) : 
    owner_(owner) {

}


ListControlImplementation::~ListControlImplementation() {

    UnregisterScrollBarEvents(owner_.VerticalScrollBar());
    UnregisterDataSourceEvents();
}


void ListControlImplementation::Initialize(const InitializeParameters& parameters) {

    owner_.SetBackgroundColor(Color::White());
    owner_.SetBorder(Frame(1));
    owner_.SetBorderColor(Color::Black());

    data_source_change_event_ = parameters.data_source_change_event;
    delegate_change_event_ = parameters.delegate_change_event;
    item_container_change_event_ = parameters.item_container_change_event;
    selection_change_event_ = parameters.selection_change_event;

    //Item container must be the first.
    InstallItemContainer(parameters.item_container);
    InstallDataSource(parameters.data_source);
    InstallDelegate(parameters.delegate);

    RegisterScrollBarEvents();

    Reload();
}


void ListControlImplementation::RegisterScrollBarEvents() {

    auto vertical_scroll_bar = owner_.VerticalScrollBar();

    vertical_scroll_bar_subscription_ = vertical_scroll_bar->ScrollEvent().Subscribe(
        std::bind(&ListControlImplementation::UpdateVisibleItems, this));
}


void ListControlImplementation::UnregisterScrollBarEvents(
    const std::shared_ptr<ScrollBar>& scroll_bar) {

    vertical_scroll_bar_subscription_.Unsubscribe();
}


void ListControlImplementation::AdjustScrollBarSmallChange() {

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


void ListControlImplementation::SetDataSource(const std::weak_ptr<ListDataSource>& data_source) {

    UnregisterDataSourceEvents();

    auto previous_data_source = data_source_;

    InstallDataSource(data_source);

    if (data_source_change_event_) {
        data_source_change_event_(previous_data_source.lock());
    }

    Reload();
}


void ListControlImplementation::InstallDataSource(
    const std::weak_ptr<ListDataSource>& data_source) {

    data_source_ = data_source;

    //Re-create item height manager once data source is changed, bacause item height manager 
    //depends on notifications of data source.
    item_height_manager_ = std::make_shared<internal::ListControlItemHeightManager>(data_source_);

    item_container_->SetSelectStrategy(CreateSelectStrategy());

    RegisterDataSourceEvents();
}


void ListControlImplementation::RegisterDataSourceEvents() {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    data_source_subscriptions_ += data_source->DataAddEvent().Subscribe(
        std::bind(
            &ListControlImplementation::ItemAdd, 
            this, 
            std::placeholders::_1));

    data_source_subscriptions_ += data_source->DataRemoveEvent().Subscribe(
        std::bind(
            &ListControlImplementation::ItemRemove,
            this, 
            std::placeholders::_1));

    data_source_subscriptions_ += data_source->DataUpdateEvent().Subscribe(
        std::bind(
            &ListControlImplementation::ItemUpdate,
            this, 
            std::placeholders::_1));
}


void ListControlImplementation::UnregisterDataSourceEvents() {

    data_source_subscriptions_.Clear();
}


void ListControlImplementation::SetDelegate(const std::weak_ptr<ListControlDelegate>& delegate) {

    auto previous_delegate = delegate_;
    
    InstallDelegate(delegate);

    if (delegate_change_event_) {
        delegate_change_event_(previous_delegate.lock());
    }

    Reload();
}


void ListControlImplementation::InstallDelegate(
    const std::weak_ptr<ListControlDelegate>& delegate) {

    delegate_ = delegate;
    item_height_manager_->ResetDelegate(delegate_);
}


void ListControlImplementation::SetItemContainer(
    const std::shared_ptr<ListItemContainer>& item_container) {

    if (item_container_ == item_container) {
        return;
    }

    auto previous_item_container = item_container_;

    item_container_subscriptions_.Clear();
    InstallItemContainer(item_container);

    if (item_container_change_event_) {
        item_container_change_event_(previous_item_container);
    }
    
    Reload();
}


void ListControlImplementation::InstallItemContainer(
    const std::shared_ptr<ListItemContainer>& item_container) {

    ZAF_EXPECT(item_container);

    item_container_ = item_container;
    item_container_->SetSelectStrategy(CreateSelectStrategy());

    item_container_subscriptions_ += item_container_->DoubleClickEvent().Subscribe(
        std::bind(
            &ListControlImplementation::OnItemContainerDoubleClick, 
            this, 
            std::placeholders::_1));

    owner_.SetScrollContent(item_container_);
}


void ListControlImplementation::OnItemContainerDoubleClick(
    const ControlDoubleClickInfo& event_info) {

    if (!item_double_click_event_) {
        return;
    }

    auto index = FindItemIndexAtPosition(event_info.Position());
    if (index) {
        item_double_click_event_(*index);
    }
}


void ListControlImplementation::SetSelectionMode(SelectionMode mode) {

    selection_mode_ = mode;

    item_container_->SetSelectStrategy(CreateSelectStrategy());

    if (selection_mode_ == SelectionMode::Single) {

        auto first_selected_index = GetFirstSelectedItemIndex();
        if (first_selected_index) {

            ReplaceSelection(*first_selected_index, 1);
            NotifySelectionChange(
                ListSelectionChangeReason::ReplaceSelection,
                *first_selected_index, 
                1);
        }
    }
    else if (selection_mode_ == SelectionMode::None) {
        UnselectAllItems();
    }
}


void ListControlImplementation::OnLayout() {

    if (!disable_on_layout_) {
        UpdateVisibleItems();
    }
}


void ListControlImplementation::OnVerticalScrollBarChange(
    const std::shared_ptr<ScrollBar>& previous_scroll_bar) {

    if (previous_scroll_bar) {
        UnregisterScrollBarEvents(previous_scroll_bar);
    }

    RegisterScrollBarEvents();
    AdjustScrollBarSmallChange();
}


void ListControlImplementation::Reload() {

    //Remove selected indexes.
    UnselectAllItems();

    //Remove all visible items.
    if (!visible_items_.empty()) {

        auto update_guard = item_container_->BeginUpdate();
        for (const auto& each_item : visible_items_) {
            item_container_->RemoveChild(each_item);
        }

        visible_items_.clear();
        first_visible_item_index_ = 0;
    }

    UpdateContentHeight();
    UpdateVisibleItems();
    AdjustScrollBarSmallChange();
}


void ListControlImplementation::UpdateContentHeight() {

    item_height_manager_->ReloadItemHeights();
    SetScrollContentHeight(item_height_manager_->GetTotalHeight());
}


void ListControlImplementation::UpdateVisibleItems() {

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


void ListControlImplementation::GetVisibleItemsRange(std::size_t& index, std::size_t& count) {

    auto visible_rect = owner_.GetVisibleScrollContentRect();
    float begin_position = visible_rect.position.y;
    float end_position = begin_position + visible_rect.size.height;

    //No need further calculation if the rect is abnormal.
    if (end_position <= begin_position) {
        index = 0;
        count = 0;
        return;
    }

    auto index_and_count = item_height_manager_->GetItemRange(begin_position, end_position);
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


void ListControlImplementation::AdjustVisibleItems(
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
        }
    }

    if (remove_tail) {
        RemoveTailVisibleItems(tail_change_count);
    }
    else {
        if (tail_change_count > 0) {
            auto new_tail_items = CreateItems(new_index + new_count - tail_change_count, tail_change_count);
            visible_items_.insert(visible_items_.end(), new_tail_items.begin(), new_tail_items.end());
        }
    }
}


void ListControlImplementation::RemoveHeadVisibleItems(std::size_t count) {

    for (std::size_t current_count = 0; current_count < count; ++current_count) {
        auto item = visible_items_.front();
        visible_items_.pop_front();
        item_container_->RemoveChild(item);
    }
}


void ListControlImplementation::RemoveTailVisibleItems(std::size_t count) {

    for (std::size_t current_count = 0; current_count < count; ++current_count) {
        auto item = visible_items_.back();
        visible_items_.pop_back();
        item_container_->RemoveChild(item);
    }
}


std::vector<std::shared_ptr<ListItem>> ListControlImplementation::CreateItems(
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


std::shared_ptr<ListItem> ListControlImplementation::CreateItem(std::size_t index) {

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

    auto item_text = delegate->GetItemText(index, item_data);
    if (!item_text.empty()) {
        list_item->SetText(item_text);
    }
    
    delegate->LoadItem(list_item, index, item_data);

    auto position_and_height = item_height_manager_->GetItemPositionAndHeight(index);
    Rect item_rect;
    item_rect.position.y = position_and_height.first;
    item_rect.size.height = position_and_height.second;
    list_item->SetRect(item_rect);

    if (item_selection_manager_.IsIndexSelected(index)) {
        list_item->SetIsSelected(true);
    }

    return list_item;
}


void ListControlImplementation::ItemAdd(const ListDataSourceDataAddInfo& event_info) {

    //Adjust scroll bar small change if there is no items before adding.
    bool need_adjust_scroll_bar_small_change = visible_items_.empty();

    bool selection_changed = item_selection_manager_.AdjustSelectionByAddingIndexes(
        event_info.index, 
        event_info.count);

    auto update_gurad = item_container_->BeginUpdate();

    float position_difference = AdjustContentHeight();

    if (event_info.index >= first_visible_item_index_ + visible_items_.size()) {
        
        UpdateVisibleItems();
    }
    else {

        if (event_info.index <= first_visible_item_index_) {
            AddItemsBeforeVisibleItems(event_info.index, event_info.count, position_difference);
        }
        else {
            AddItemsInMiddleOfVisibleItems(event_info.index, event_info.count, position_difference);
        }
    }

    if (selection_changed) {
        NotifySelectionChange(ListSelectionChangeReason::ItemChange, 0, 0);
    }

    if (need_adjust_scroll_bar_small_change) {
        AdjustScrollBarSmallChange();
    }
}


void ListControlImplementation::AddItemsBeforeVisibleItems(
    std::size_t index,
    std::size_t count,
    float position_difference) {

    first_visible_item_index_ += count;
    AdjustVisibleItemPositions(0, position_difference);
    UpdateVisibleItems();
}


void ListControlImplementation::AddItemsInMiddleOfVisibleItems(
    std::size_t index,
    std::size_t count,
    float position_difference) {

    std::size_t insert_index = index - first_visible_item_index_;
    std::size_t need_adjust_position_count = visible_items_.size() - insert_index;

    if (count >= need_adjust_position_count) {

        RemoveTailVisibleItems(need_adjust_position_count);
        UpdateVisibleItems();
    }
    else {

        AdjustVisibleItemPositions(insert_index, position_difference);

        auto new_items = CreateItems(index, count);
        visible_items_.insert(
            std::next(visible_items_.begin(), insert_index),
            new_items.begin(), 
            new_items.end());
    }
}


void ListControlImplementation::ItemRemove(const ListDataSourceDataRemoveInfo& event_info) {

    bool selection_changed = item_selection_manager_.AdjustSelectionByRemovingIndexes(
        event_info.index,
        event_info.count);

    auto update_gurad = item_container_->BeginUpdate();

    float position_difference = AdjustContentHeight();

    if (event_info.index >= first_visible_item_index_ + visible_items_.size()) {
        return;
    }

    if (event_info.index < first_visible_item_index_) {
        RemoveItemsBeforeVisibleItems(event_info.index, event_info.count, position_difference);
    }
    else {
        RemoveItemsInMiddleOfVisibleItems(event_info.index, event_info.count, position_difference);
    }

    if (selection_changed) {
        NotifySelectionChange(ListSelectionChangeReason::ItemChange, 0, 0);
    }
}


void ListControlImplementation::RemoveItemsBeforeVisibleItems(
    std::size_t index,
    std::size_t count,
    float position_difference) {

    first_visible_item_index_ -= count;
    AdjustVisibleItemPositions(0, position_difference);
    UpdateVisibleItems();
}


void ListControlImplementation::RemoveItemsInMiddleOfVisibleItems(
    std::size_t index,
    std::size_t count,
    float position_difference) {

    std::size_t remove_index = index - first_visible_item_index_;
    std::size_t remove_count = (std::min)(count, visible_items_.size() - remove_index);

    auto begin_erase_iterator = std::next(visible_items_.begin(), remove_index);
    auto end_erase_iterator = std::next(begin_erase_iterator, remove_count);

    for (auto iterator = begin_erase_iterator; iterator != end_erase_iterator; ++iterator) {
        item_container_->RemoveChild(*iterator);
    }

    visible_items_.erase(begin_erase_iterator, end_erase_iterator);

    AdjustVisibleItemPositions(remove_index, position_difference);
    UpdateVisibleItems();
}


void ListControlImplementation::ItemUpdate(const ListDataSourceDataUpdateInfo& event_info) {

    auto update_guard = item_container_->BeginUpdate();

    float position_difference = AdjustContentHeight();

    if (event_info.index >= first_visible_item_index_ + visible_items_.size()) {
        return;
    }

    AdjustVisibleItemPositionsByUpdatingItems(
        event_info.index,
        event_info.count, 
        position_difference);

    UpdateVisibleItemsByUpdatingItems(event_info.index, event_info.count);
}


void ListControlImplementation::AdjustVisibleItemPositionsByUpdatingItems(
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


void ListControlImplementation::UpdateVisibleItemsByUpdatingItems(
    std::size_t index, 
    std::size_t count) {

    std::size_t end_update_index = index + count;
    std::size_t end_visible_item_index = first_visible_item_index_ + visible_items_.size();

    if ((index >= end_visible_item_index) || (first_visible_item_index_ >= end_update_index)) {
        return;
    }

    std::size_t intersect_begin_update_index = std::max(index, first_visible_item_index_);
    std::size_t intersect_end_update_index = (std::min)(end_update_index, end_visible_item_index);

    for (std::size_t current_index = intersect_begin_update_index; current_index < intersect_end_update_index; ++current_index) {

        std::size_t visible_item_index = current_index - first_visible_item_index_;

        auto current_item = visible_items_[visible_item_index];
        item_container_->RemoveChild(current_item);

        auto new_item = CreateItem(current_index);
        item_container_->AddChild(new_item);
        visible_items_[visible_item_index] = new_item;
    }
}


float ListControlImplementation::AdjustContentHeight() {

    float old_total_height = current_total_height_;
    float new_total_height = item_height_manager_->GetTotalHeight();

    if (old_total_height != new_total_height) {

        //Disable OnLayout() for preventing from reentering.
        disable_on_layout_ = true;
        SetScrollContentHeight(new_total_height);
        disable_on_layout_ = false;

        return new_total_height - old_total_height;
    }
    else {
        return 0;
    }
}


void ListControlImplementation::SetScrollContentHeight(float height) {

    current_total_height_ = height;
    item_container_->SetFixedHeight(height);
}


void ListControlImplementation::AdjustVisibleItemPositions(
    std::size_t begin_adjust_index, 
    float difference) {

    for (std::size_t index = begin_adjust_index; index < visible_items_.size(); ++index) {

        const auto& item = visible_items_[index];
        auto rect = item->Rect();
        rect.position.y += difference;
        item->SetRect(rect);
    }
}


void ListControlImplementation::SelectAllItems() {

    std::size_t item_count = GetItemCount();
    std::size_t selected_count = GetSelectedItemCount();
    if (selected_count != item_count) {

        ReplaceSelection(0, item_count);
        NotifySelectionChange(ListSelectionChangeReason::ReplaceSelection, 0, item_count);
    }
}


void ListControlImplementation::UnselectAllItems() {

    std::size_t selected_count = GetSelectedItemCount();
    if (selected_count != 0) {

        ReplaceSelection(0, 0);
        NotifySelectionChange(ListSelectionChangeReason::ReplaceSelection, 0, 0);
    }
}


void ListControlImplementation::SelectItemAtIndex(std::size_t index) {

    if ((index >= GetItemCount()) || IsItemSelectedAtIndex(index)) {
        return;
    }

    switch (GetSelectionMode()) {

    case SelectionMode::Single:
        ReplaceSelection(index, 1);
        NotifySelectionChange(ListSelectionChangeReason::ReplaceSelection, index, 1);
        break;

    case SelectionMode::SimpleMultiple:
    case SelectionMode::ExtendedMultiple:
        AddSelection(index, 1);
        NotifySelectionChange(ListSelectionChangeReason::AddSelection, index, 1);
        break;

    default:
        return;
    }
}


void ListControlImplementation::UnselectItemAtIndex(std::size_t index) {

    if ((index >= GetItemCount()) || !IsItemSelectedAtIndex(index)) {
        return;
    }

    RemoveSelection(index, 1);
    NotifySelectionChange(ListSelectionChangeReason::RemoveSelection, index, 1);
}


std::size_t ListControlImplementation::GetItemCount() {
    return item_height_manager_->GetItemCount();
}


std::size_t ListControlImplementation::GetSelectedItemCount() {
    return item_selection_manager_.GetAllSelectedCount();
}


std::optional<std::size_t> ListControlImplementation::GetFirstSelectedItemIndex() {
    return item_selection_manager_.GetFirstSelectedIndex();
}


std::vector<std::size_t> ListControlImplementation::GetAllSelectedItemIndexes() {
    return item_selection_manager_.GetAllSelectedIndexes();
}


bool ListControlImplementation::IsItemSelectedAtIndex(std::size_t index) {
    return item_selection_manager_.IsIndexSelected(index);
}


void ListControlImplementation::ScrollToItemAtIndex(std::size_t index) {

    auto position_and_height = item_height_manager_->GetItemPositionAndHeight(index);

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


std::optional<std::size_t> ListControlImplementation::FindItemIndexAtPosition(
    const Point& position) {

    auto visible_scroll_content_rect = owner_.GetVisibleScrollContentRect();

    if (position.x < 0 || position.x > visible_scroll_content_rect.size.width) {
        return std::nullopt;
    }

    float adjusted_position = position.y + visible_scroll_content_rect.position.y;
    return item_height_manager_->GetItemIndex(adjusted_position);
}


std::optional<std::size_t> ListControlImplementation::GetListItemIndex(
    const std::shared_ptr<ListItem>& item) {

    for (auto index : zaf::Range(visible_items_.size())) {

        if (visible_items_[index] == item) {
            return index + first_visible_item_index_;
        }
    }

    return std::nullopt;
}


void ListControlImplementation::ReplaceSelection(std::size_t index, std::size_t count) {

    item_selection_manager_.ReplaceSelection(index, count);

    for (std::size_t visible_item_index = 0; 
         visible_item_index < visible_items_.size(); 
         ++visible_item_index) {

        const auto& each_visible_item = visible_items_[visible_item_index];

        std::size_t item_index = first_visible_item_index_ + visible_item_index;
        bool is_selected = (index <= item_index) && (item_index < index + count);
        each_visible_item->SetIsSelected(is_selected);
    }
}


bool ListControlImplementation::RevertSelection(std::size_t index) {

    bool is_selected = item_selection_manager_.RevertSelection(index);

    if ((first_visible_item_index_ <= index) && (index < first_visible_item_index_ + visible_items_.size())) {

        const auto& visible_item = visible_items_[index - first_visible_item_index_];
        visible_item->SetIsSelected(!visible_item->IsSelected());
    }

    return is_selected;
}


void ListControlImplementation::AddSelection(std::size_t index, std::size_t count) {
    ChangeSelection(index, count, true);
}


void ListControlImplementation::RemoveSelection(std::size_t index, std::size_t count) {
    ChangeSelection(index, count, false);
}


void ListControlImplementation::ChangeSelection(std::size_t index, std::size_t count, bool is_add) {

    if (is_add) {
        item_selection_manager_.AddSelection(index, count);
    }
    else {
        item_selection_manager_.RemoveSelection(index, count);
    }

    std::size_t intersect_begin_index = std::max(index, first_visible_item_index_);
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


void ListControlImplementation::NotifySelectionChange(
    ListSelectionChangeReason change_type,
    std::size_t index,
    std::size_t count) {

    if (selection_change_event_) {
        selection_change_event_(change_type, index, count);
    }
}


std::shared_ptr<internal::ListControlSelectStrategy> 
    ListControlImplementation::CreateSelectStrategy() {

    std::shared_ptr<internal::ListControlSelectStrategy> select_strategy;

    switch (GetSelectionMode()) {
    case SelectionMode::Single:
        select_strategy = std::make_shared<internal::ListControlSingleSelectStrategy>();
        break;

    case SelectionMode::SimpleMultiple:
        select_strategy = std::make_shared<internal::ListControlSimpleMultipleSelectStrategy>();
        break;

    case SelectionMode::ExtendedMultiple:
        select_strategy = std::make_shared<internal::ListControlExtendedMultipleSelectStrategy>();
        break;

    default:
        select_strategy = std::make_shared<internal::ListControlNoSelectStrategy>();
        break;
    }

    select_strategy->SetListControl(shared_from_this());
    select_strategy->SetItemHeightManager(item_height_manager_);
    return select_strategy;
}


}