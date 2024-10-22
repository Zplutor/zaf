#include <zaf/internal/list/list_visible_item_manager.h>
#include <zaf/internal/list/list_control_core.h>
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


bool ListVisibleItemManager::HasVisibleItem() const noexcept {
    return !visible_items_.empty();
}


std::shared_ptr<ListItem> ListVisibleItemManager::GetVisibleItemAtIndex(
    std::size_t index) const noexcept {

    if (index < first_visible_item_index_ ||
        index >= first_visible_item_index_ + visible_items_.size()) {
        return nullptr;
    }

    auto index_in_visible_items = index - first_visible_item_index_;
    return visible_items_[index_in_visible_items];
}


std::optional<std::size_t> ListVisibleItemManager::GetIndexOfVisibleItem(
    const ListItem& item) const noexcept {

    for (auto index : Range(0, visible_items_.size())) {

        if (visible_items_[index].get() == &item) {
            return index + first_visible_item_index_;
        }
    }
    return std::nullopt;
}


void ListVisibleItemManager::HandleDataAdded(const Range& added_range, float height_difference) {

    auto first_need_update_index = AddVisibleItems(added_range);
    if (first_need_update_index) {
        AdjustVisibleItemPositions(*first_need_update_index, height_difference);
    }
    UpdateVisibleItems();
}


//Returns the index of the first visible item which needs to be adjusted its position.
std::optional<std::size_t> ListVisibleItemManager::AddVisibleItems(const Range& added_range) {

    if (added_range.index >= first_visible_item_index_ + visible_items_.size()) {
        return std::nullopt;
    }

    if (added_range.index <= first_visible_item_index_) {
        first_visible_item_index_ += added_range.length;
        return 0;
    }

    return AddMiddleVisibleItems(added_range);
}


std::optional<std::size_t> ListVisibleItemManager::AddMiddleVisibleItems(
    const Range& added_range) {

    std::size_t insert_index = added_range.index - first_visible_item_index_;
    std::size_t need_adjust_position_count = visible_items_.size() - insert_index;

    if (added_range.length >= need_adjust_position_count) {
        RemoveTailVisibleItems(need_adjust_position_count);
        return std::nullopt;
    }
    else {

        auto new_items = CreateItems(added_range.index, added_range.length);
        visible_items_.insert(
            std::next(visible_items_.begin(), insert_index),
            new_items.begin(),
            new_items.end());

        return insert_index + new_items.size();
    }
}


void ListVisibleItemManager::HandleDataRemoved(
    const Range& removed_range, 
    float height_difference) {

    auto first_need_update_index = RemoveVisibleItems(removed_range);
    if (first_need_update_index) {
        AdjustVisibleItemPositions(*first_need_update_index, height_difference);
        UpdateVisibleItems();
    }
}


//Returns the index of the first visible item which needs to be adjusted its position.
std::optional<std::size_t> ListVisibleItemManager::RemoveVisibleItems(const Range& removed_range) {

    if (removed_range.index >= first_visible_item_index_ + visible_items_.size()) {
        return std::nullopt;
    }

    if (removed_range.index < first_visible_item_index_) {
        first_visible_item_index_ -= removed_range.length;
        return 0;
    }

    return RemoveMiddleVisibleItems(removed_range);
}


std::size_t ListVisibleItemManager::RemoveMiddleVisibleItems(const Range& removed_range) {

    std::size_t remove_index = removed_range.index - first_visible_item_index_;
    std::size_t remove_count = 
        (std::min)(removed_range.length, visible_items_.size() - remove_index);

    auto begin_erase_iterator = std::next(visible_items_.begin(), remove_index);
    auto end_erase_iterator = std::next(begin_erase_iterator, remove_count);

    const auto& item_container = Context().Owner().ItemContainer();
    for (auto iterator = begin_erase_iterator; iterator != end_erase_iterator; ++iterator) {
        item_container->RemoveChild(*iterator);
    }

    visible_items_.erase(begin_erase_iterator, end_erase_iterator);
    return remove_index;
}


void ListVisibleItemManager::HandleDataUpdated(
    const Range& updated_range, 
    float height_difference) {

    if (updated_range.Index() >= first_visible_item_index_ + visible_items_.size()) {
        return;
    }

    AdjustVisibleItemPositionsByUpdatingItems(updated_range, height_difference);
    UpdateVisibleItemsByUpdatingItems(updated_range);
}


void ListVisibleItemManager::AdjustVisibleItemPositionsByUpdatingItems(
    const Range& updated_range,
    float position_difference) {

    if (position_difference == 0) {
        return;
    }

    std::size_t end_update_index = updated_range.EndIndex();
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


void ListVisibleItemManager::UpdateVisibleItemsByUpdatingItems(const Range& updated_range) {

    std::size_t end_update_index = updated_range.EndIndex();
    std::size_t end_visible_item_index = first_visible_item_index_ + visible_items_.size();

    if ((updated_range.index >= end_visible_item_index) || 
        (first_visible_item_index_ >= end_update_index)) {
        return;
    }

    std::size_t intersect_begin_update_index = 
        (std::max)(updated_range.index, first_visible_item_index_);
    std::size_t intersect_end_update_index = (std::min)(end_update_index, end_visible_item_index);

    std::vector<std::shared_ptr<ListItem>> new_items;
    const auto& item_container = Context().Owner().ItemContainer();
    for (std::size_t current_index = intersect_begin_update_index;
        current_index < intersect_end_update_index;
        ++current_index) {

        std::size_t visible_item_index = current_index - first_visible_item_index_;

        auto current_item = visible_items_[visible_item_index];
        item_container->RemoveChild(current_item);

        auto new_item = CreateItem(current_index);
        item_container->AddChild(new_item);
        visible_items_[visible_item_index] = new_item;

        new_items.push_back(new_item);
    }

    //RecoverLastFocusedItem(new_items);
}


void ListVisibleItemManager::AdjustVisibleItemPositions(
    std::size_t begin_adjust_index,
    float difference) {

    for (auto index : Range(begin_adjust_index, visible_items_.size())) {

        const auto& item = visible_items_[index];
        auto rect = item->Rect();
        rect.position.y += difference;
        item->SetRect(rect);
    }
}


void ListVisibleItemManager::HandleDataMoved(std::size_t previous_index, std::size_t new_index) {

    RemoveVisibleItems(Range{ previous_index, 1 });
    AddVisibleItems(Range{ new_index, 1 });

    auto& item_height_manager = Context().ItemHeightManager();
    for (auto visible_index : Range{ 0, visible_items_.size() }) {

        auto item_index = visible_index + first_visible_item_index_;
        auto [position, height] = item_height_manager.GetItemPositionAndHeight(item_index);
        visible_items_[visible_index]->SetY(position);
    }

    UpdateVisibleItems();
}


void ListVisibleItemManager::UpdateVisibleItems() {

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


void ListVisibleItemManager::GetVisibleItemsRange(std::size_t& index, std::size_t& count) {

    auto visible_rect = Context().Owner().ScrollBox().GetVisibleScrollContentRect();
    float begin_position = visible_rect.position.y;
    float end_position = begin_position + visible_rect.size.height;

    //No need further calculation if the rect is abnormal.
    if (end_position <= begin_position) {
        index = 0;
        count = 0;
        return;
    }

    auto& item_height_manager = Context().ItemHeightManager();

    auto index_and_count = item_height_manager.GetItemRange(begin_position, end_position);
    index = index_and_count.first;
    count = index_and_count.second;

    if (index > 2) {
        index -= 2;
    }
    else {
        index = 0;
    }

    count += 2;
    std::size_t max_count = item_height_manager.GetItemCount() - index;
    if (count > max_count) {
        count = max_count;
    }
}


void ListVisibleItemManager::AdjustVisibleItems(
    std::size_t new_index,
    std::size_t new_count,
    bool remove_head,
    std::size_t head_change_count,
    bool remove_tail,
    std::size_t tail_change_count) {

    auto update_guard = Context().Owner().ItemContainer()->BeginUpdate();

    if (remove_head) {
        RemoveHeadVisibleItems(head_change_count);
    }
    else {
        if (head_change_count > 0) {
            auto new_head_items = CreateItems(new_index, head_change_count);
            visible_items_.insert(visible_items_.begin(), new_head_items.begin(), new_head_items.end());
            //RecoverLastFocusedItem(new_head_items);
        }
    }

    if (remove_tail) {
        RemoveTailVisibleItems(tail_change_count);
    }
    else {
        if (tail_change_count > 0) {
            auto new_tail_items = CreateItems(new_index + new_count - tail_change_count, tail_change_count);
            visible_items_.insert(visible_items_.end(), new_tail_items.begin(), new_tail_items.end());
            //RecoverLastFocusedItem(new_tail_items);
        }
    }
}


void ListVisibleItemManager::RemoveHeadVisibleItems(std::size_t count) {

    const auto& item_container = Context().Owner().ItemContainer();

    for (auto index : Range(0, count)) {
        auto item = visible_items_.front();
        visible_items_.pop_front();
        item_container->RemoveChild(item);
    }
}


void ListVisibleItemManager::RemoveTailVisibleItems(std::size_t count) {

    const auto& item_container = Context().Owner().ItemContainer();

    for (auto index : Range(0, count)) {
        auto item = visible_items_.back();
        visible_items_.pop_back();
        item_container->RemoveChild(item);
    }
}


std::vector<std::shared_ptr<ListItem>> ListVisibleItemManager::CreateItems(
    std::size_t index,
    std::size_t count) {

    std::vector<std::shared_ptr<ListItem>> items;
    items.reserve(count);

    const auto& item_container = Context().Owner().ItemContainer();
    for (auto current_index : Range(index, count)) {

        auto new_item = CreateItem(current_index);
        if (!new_item) {
            continue;
        }

        item_container->AddChild(new_item);
        items.push_back(new_item);
    }

    return items;
}


std::shared_ptr<ListItem> ListVisibleItemManager::CreateItem(std::size_t index) {

    auto data_source = Context().Owner().DataSource();
    if (!data_source) {
        return nullptr;
    }

    auto delegate = Context().Owner().Delegate();
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

    auto position_and_height = Context().ItemHeightManager().GetItemPositionAndHeight(index);
    Rect item_rect;
    item_rect.position.y = position_and_height.first;
    item_rect.size.height = position_and_height.second;
    list_item->SetRect(item_rect);

    list_item->SetIsSelected(Context().SelectionStore().IsIndexSelected(index));
    return list_item;
}


void ListVisibleItemManager::ChangeVisibleItemSelection(
    ListSelectionChangeReason change_reason, 
    const Range& changed_range) {

    const auto& item_container = Context().Owner().ItemContainer();
    auto update_guard = item_container->BeginUpdate();

    switch (change_reason) {
    case ListSelectionChangeReason::AddSelection:
        ChangeVisibleItemSelectionByAddingOrRemoving(changed_range, true);
        break;
    case ListSelectionChangeReason::RemoveSelection:
        ChangeVisibleItemSelectionByAddingOrRemoving(changed_range, false);
        break;
    case ListSelectionChangeReason::ReplaceSelection:
        ChangeVisibleItemSelectionByReplacing(changed_range);
        break;
    default:
        ChangeVisibleItemSelectionByRefreshing();
        break;
    }
}


void ListVisibleItemManager::ChangeVisibleItemSelectionByAddingOrRemoving(
    const Range& changed_range, 
    bool is_adding) {

    std::size_t intersect_begin_index = (std::max)(changed_range.index, first_visible_item_index_);
    std::size_t intersect_end_index =
        (std::min)(changed_range.EndIndex(), first_visible_item_index_ + visible_items_.size());

    if (intersect_begin_index >= intersect_end_index) {
        return;
    }

    for (auto item_index : Range::FromIndexPair(intersect_begin_index, intersect_end_index)) {
        auto visible_item_index = item_index - first_visible_item_index_;
        visible_items_[visible_item_index]->SetIsSelected(is_adding);
    }
}


void ListVisibleItemManager::ChangeVisibleItemSelectionByReplacing(const Range& changed_range) {

    for (auto index : Range(0, visible_items_.size())) {

        const auto& visible_item = visible_items_[index];

        std::size_t item_index = first_visible_item_index_ + index;
        bool is_selected = changed_range.Contains(item_index);
        
        visible_item->SetIsSelected(is_selected);
    }
}


void ListVisibleItemManager::ChangeVisibleItemSelectionByRefreshing() {

    auto& selection_store = Context().SelectionStore();

    for (auto index : Range(0, visible_items_.size())) {

        const auto& visible_item = visible_items_[index];

        std::size_t item_index = first_visible_item_index_ + index;
        bool is_selected = selection_store.IsIndexSelected(item_index);

        visible_item->SetIsSelected(is_selected);
    }
}


void ListVisibleItemManager::ClearVisibleItems() {

    if (visible_items_.empty()) {
        return;
    }

    const auto& item_container = Context().Owner().ItemContainer();
    auto update_guard = item_container->BeginUpdate();

    for (const auto& each_item : visible_items_) {
        item_container->RemoveChild(each_item);
    }

    visible_items_.clear();
    first_visible_item_index_ = 0;
}

}