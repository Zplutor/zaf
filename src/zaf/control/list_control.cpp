#include <zaf/control/list_control.h>
#include <algorithm>
#include <zaf/base/event_utility.h>
#include <zaf/control/internal/list_control_extended_multiple_select_strategy.h>
#include <zaf/control/internal/list_control_item_height_manager.h>
#include <zaf/control/internal/list_control_item_selection_manager.h>
#include <zaf/control/internal/list_control_simple_multiple_select_strategy.h>
#include <zaf/control/internal/list_control_single_select_strategy.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>
#include <zaf/serialization/types.h>
#include <zaf/window/message/mouse_message.h>

#undef max
#undef min

namespace zaf {

static const wchar_t* const kSelectionChangeEventPropertyName = L"SelectionChangeEvent";
static const wchar_t* const kSelectionModePropertyName = L"SelectionMode";

static void CalculateRangeDifference(
    std::size_t new_index, 
    std::size_t new_count, 
    std::size_t old_index,
    std::size_t old_count,
    bool& remove_head,
    std::size_t& head_change_count,
    bool& remove_tail,
    std::size_t& tail_change_count);

ListControl::ListControl() : 
    item_source_(std::make_shared<ItemSource>()),
    first_visible_item_index_(0) {

    item_height_manager_ = std::make_shared<internal::ListControlItemHeightManager>(item_source_);
    item_selection_manager_ = std::make_unique<internal::ListControlItemSelectionManager>();
}


ListControl::~ListControl() {

    GetVerticalScrollBar()->GetScrollEvent().RemoveListenersWithTag(reinterpret_cast<std::uintptr_t>(this));
    UninitializeItemSource();
}


void ListControl::Initialize() {

    __super::Initialize();

    SetBackgroundColor(Color::White);
    SetBorder(1);
    SetBorderColor(Color::Black);

    item_container_ = Create<ItemContainer>();
    item_container_->SetSelectStrategy(CreateSelectStrategy());
    SetScrollContentControl(item_container_);

    InitializeScrollBar();
    InitializeItemSource();
    Reload();
}


void ListControl::InitializeScrollBar() {

    auto vertical_scroll_bar = GetVerticalScrollBar();
    vertical_scroll_bar->GetScrollEvent().AddListenerWithTag(
        reinterpret_cast<std::uintptr_t>(this), 
        std::bind(&ListControl::UpdateVisibleItems, this));
}


void ListControl::InitializeItemSource() {

    auto item_source = GetItemSource();
    auto tag = reinterpret_cast<std::uintptr_t>(this);

    item_source->GetItemAddEvent().AddListenerWithTag(
        tag, 
        std::bind(&ListControl::ItemAdd, this, std::placeholders::_2, std::placeholders::_3));

    item_source->GetItemRemoveEvent().AddListenerWithTag(
        tag,
        std::bind(&ListControl::ItemRemove, this, std::placeholders::_2, std::placeholders::_3));

    item_source->GetItemUpdateEvent().AddListenerWithTag(
        tag,
        std::bind(&ListControl::ItemUpdate, this, std::placeholders::_2, std::placeholders::_3));
}


void ListControl::UninitializeItemSource() {

    auto item_source = GetItemSource();
    auto tag = reinterpret_cast<std::uintptr_t>(this);

    item_source->GetItemAddEvent().RemoveListenersWithTag(tag);
    item_source->GetItemRemoveEvent().RemoveListenersWithTag(tag);
    item_source->GetItemUpdateEvent().RemoveListenersWithTag(tag);
}


void ListControl::Layout(const Rect& previous_rect) {

    __super::Layout(previous_rect);
    UpdateVisibleItems();
}


void ListControl::VerticalScrollBarChange(const std::shared_ptr<ScrollBar>& previous_scroll_bar) {

    __super::VerticalScrollBarChange(previous_scroll_bar);

    previous_scroll_bar->GetScrollEvent().RemoveListenersWithTag(reinterpret_cast<std::uintptr_t>(this));
    InitializeScrollBar();
}


void ListControl::SetItemSource(const std::shared_ptr<ItemSource>& item_source) {

    UninitializeItemSource();

    auto previous_item_source = item_source_;
    if (item_source != nullptr) {
        item_source_ = item_source;
    }
    else {
        item_source_ = std::make_shared<ItemSource>();
    }

    item_height_manager_ = std::make_shared<internal::ListControlItemHeightManager>(item_source_);
    item_container_->SetSelectStrategy(CreateSelectStrategy());
    InitializeItemSource();

    ItemSourceChange(previous_item_source);
    Reload();
}


void ListControl::SetItemContainer(const std::shared_ptr<ItemContainer>& item_container) {

    if (item_container_ == item_container) {
        return;
    }

    auto previous_item_container = item_container_;

    item_container_ = item_container;
    if (item_container_ == nullptr) {
        item_container_ = std::make_shared<ItemContainer>();
    }

    item_container_->SetSelectStrategy(CreateSelectStrategy());
    SetScrollContentControl(item_container_);

    ItemContainerChange(previous_item_container);
    Reload();
}


void ListControl::Reload() {

    //Remove selected indexes.
    UnselectAllItems();

    //Remove all visible items.
    if (! visible_items_.empty()) {

        UpdateGuard update_guard(*item_container_);
        for (const auto& each_item : visible_items_) {
            item_container_->RemoveChild(each_item);
        }

        visible_items_.clear();
        first_visible_item_index_ = 0;
    }

    UpdateContentHeight();
    UpdateVisibleItems();
}


void ListControl::UpdateContentHeight() {

    item_height_manager_->LoadItemHeights();
    SetScrollContentSize(Size(0, item_height_manager_->GetTotalHeight()));
}


void ListControl::UpdateVisibleItems() {

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


void ListControl::GetVisibleItemsRange(std::size_t& index, std::size_t& count) const {

    auto visible_rect = GetVisibleScrollContentRect();
    float begin_position = visible_rect.position.y;
    float end_position = begin_position + visible_rect.size.height;

    //No need further calculation if the rect is abnormal.
    if (end_position <= begin_position) {
        index = 0;
        count = 0;
        return;
    }

    auto index_and_count = item_height_manager_->GetItemIndexAndCount(begin_position, end_position);
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


void ListControl::AdjustVisibleItems(
    std::size_t new_index,
    std::size_t new_count,
    bool remove_head,
    std::size_t head_change_count,
    bool remove_tail,
    std::size_t tail_change_count) {

    UpdateGuard update_guard(*item_container_);

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


void ListControl::RemoveHeadVisibleItems(std::size_t count) {

    for (std::size_t current_count = 0; current_count < count; ++current_count) {
        auto item = visible_items_.front();
        visible_items_.pop_front();
        item_container_->RemoveChild(item);
    }
}


void ListControl::RemoveTailVisibleItems(std::size_t count) {

    for (std::size_t current_count = 0; current_count < count; ++current_count) {
        auto item = visible_items_.back();
        visible_items_.pop_back();
        item_container_->RemoveChild(item);
    }
}


const std::vector<std::shared_ptr<ListControl::Item>> ListControl::CreateItems(std::size_t index, std::size_t count) {

    std::vector<std::shared_ptr<Item>> items;
    items.reserve(count);

    auto item_source = GetItemSource();
    for (std::size_t current_index = index; current_index < index + count; ++current_index) {

        auto new_item = CreateItem(current_index);
        item_container_->AddChild(new_item);
        items.push_back(new_item);
    }

    return items;
}


const std::shared_ptr<ListControl::Item> ListControl::CreateItem(std::size_t index) const {

    const auto& item_source = GetItemSource();

    auto new_item = item_source->CreateItem(index);
    if (new_item == nullptr) {
        new_item = Create<Item>();
    }

    item_source->LoadItem(index, new_item);

    auto position_and_height = item_height_manager_->GetItemPositionAndHeight(index);
    Rect item_rect;
    item_rect.position.y = position_and_height.first;
    item_rect.size.height = position_and_height.second;
    new_item->SetRect(item_rect);

    if (item_selection_manager_->IsIndexSelected(index)) {
        new_item->SetIsSelected(true);
    }

    return new_item;
}


void ListControl::ItemAdd(std::size_t index, std::size_t count) {

    bool selection_changed = item_selection_manager_->AdjustSelectionByAddingIndexes(index, count);

    UpdateGuard update_gurad(*item_container_);

    float position_difference = AdjustContentHeight();

    if (index >= first_visible_item_index_ + visible_items_.size()) {
        return;
    }

    if (index <= first_visible_item_index_) {
        AddItemsBeforeVisibleItems(index, count, position_difference);
    }
    else {
        AddItemsInMiddleOfVisibleItems(index, count, position_difference);
    }

    if (selection_changed) {
        NotifySelectionChange();
    }
}


void ListControl::AddItemsBeforeVisibleItems(std::size_t index, std::size_t count, float position_difference) {

    first_visible_item_index_ += count;
    AdjustVisibleItemPositions(0, position_difference);
    UpdateVisibleItems();
}


void ListControl::AddItemsInMiddleOfVisibleItems(std::size_t index, std::size_t count, float position_difference) {

    std::size_t insert_index = index - first_visible_item_index_;
    std::size_t need_adjust_position_count = visible_items_.size() - insert_index;

    if (count >= need_adjust_position_count) {

        RemoveTailVisibleItems(need_adjust_position_count);
        UpdateVisibleItems();
    }
    else {

        RemoveTailVisibleItems(count);
        AdjustVisibleItemPositions(insert_index, position_difference);

        auto new_items = CreateItems(index, count);
        visible_items_.insert(std::next(visible_items_.begin(), insert_index), new_items.begin(), new_items.end());
    }
}


void ListControl::ItemRemove(std::size_t index, std::size_t count) {

    bool selection_changed = item_selection_manager_->AdjustSelectionByRemovingIndexes(index, count);

    UpdateGuard update_gurad(*item_container_);

    float position_difference = AdjustContentHeight();

    if (index >= first_visible_item_index_ + visible_items_.size()) {
        return;
    }

    if (index < first_visible_item_index_) {
        RemoveItemsBeforeVisibleItems(index, count, position_difference);
    }
    else {
        RemoveItemsInMiddleOfVisibleItems(index, count, position_difference);
    }

    if (selection_changed) {
        NotifySelectionChange();
    }
}


void ListControl::RemoveItemsBeforeVisibleItems(std::size_t index, std::size_t count, float position_difference) {

    first_visible_item_index_ -= count;
    AdjustVisibleItemPositions(0, position_difference);
    UpdateVisibleItems();
}


void ListControl::RemoveItemsInMiddleOfVisibleItems(std::size_t index, std::size_t count, float position_difference) {

    std::size_t remove_index = index - first_visible_item_index_;
    std::size_t remove_count = std::min(count, visible_items_.size() - remove_index);

    auto begin_erase_iterator = std::next(visible_items_.begin(), remove_index);
    auto end_erase_iterator = std::next(begin_erase_iterator, remove_count);

    for (auto iterator = begin_erase_iterator; iterator != end_erase_iterator; ++iterator) {
        item_container_->RemoveChild(*iterator);
    }

    visible_items_.erase(begin_erase_iterator, end_erase_iterator);

    AdjustVisibleItemPositions(remove_index, position_difference);
    UpdateVisibleItems();
}


void ListControl::ItemUpdate(std::size_t index, std::size_t count) {

    UpdateGuard update_guard(*item_container_);

    float position_difference = AdjustContentHeight();

    if (index >= first_visible_item_index_ + visible_items_.size()) {
        return;
    }

    AdjustVisibleItemPositionsByUpdatingItems(index, count, position_difference);
    UpdateVisibleItemsByUpdatingItems(index, count);
}


void ListControl::AdjustVisibleItemPositionsByUpdatingItems(std::size_t index, std::size_t count, float position_difference) {

    if (position_difference == 0) {
        return;
    }

    std::size_t end_update_index = index + count;
    std::size_t begin_adjust_index = InvalidIndex;

    if (end_update_index <= first_visible_item_index_) {
        begin_adjust_index = 0;
    }
    else if (end_update_index < first_visible_item_index_ + visible_items_.size()) {
        begin_adjust_index = end_update_index - first_visible_item_index_;
    }

    AdjustVisibleItemPositions(begin_adjust_index, position_difference);
    UpdateVisibleItems();
}


void ListControl::UpdateVisibleItemsByUpdatingItems(std::size_t index, std::size_t count) {

    std::size_t end_update_index = index + count;
    std::size_t end_visible_item_index = first_visible_item_index_ + visible_items_.size();

    if ((index >= end_visible_item_index) || (first_visible_item_index_ >= end_update_index)) {
        return;
    }

    std::size_t intersect_begin_update_index = std::max(index, first_visible_item_index_);
    std::size_t intersect_end_update_index = std::min(end_update_index, end_visible_item_index);

    for (std::size_t current_index = intersect_begin_update_index; current_index < intersect_end_update_index; ++current_index) {

        std::size_t visible_item_index = current_index - first_visible_item_index_;

        auto current_item = visible_items_[visible_item_index];
        item_container_->RemoveChild(current_item);

        auto new_item = CreateItem(current_index);
        item_container_->AddChild(new_item);
        visible_items_[visible_item_index] = new_item;
    }
}


float ListControl::AdjustContentHeight() {

    float old_total_height = GetScrollContentSize().height;
    float new_total_height = item_height_manager_->GetTotalHeight();

    if (old_total_height != new_total_height) {

        SetScrollContentSize(Size(0, new_total_height));
        return new_total_height - old_total_height;
    }
    else {
        return 0;
    }
}


void ListControl::AdjustVisibleItemPositions(std::size_t begin_adjust_index, float difference) {

    for (std::size_t index = begin_adjust_index; index < visible_items_.size(); ++index) {

        const auto& item = visible_items_[index];
        auto rect = item->GetRect();
        rect.position.y += difference;
        item->SetRect(rect);
    }
}


std::size_t ListControl::GetItemCount() const {
    return item_height_manager_->GetItemCount();
}


const std::shared_ptr<ListControl::Item> ListControl::GetItemAtIndex(std::size_t index) const {

    if (index >= GetItemCount()) {
        return nullptr;
    }

    if ((index >= first_visible_item_index_) && (index < first_visible_item_index_ + visible_items_.size())) {
        return visible_items_[index - first_visible_item_index_];
    }
    
    return CreateItem(index);
}


ListControl::SelectionMode ListControl::GetSelectionMode() const {

    auto select_mode = GetPropertyMap().TryGetProperty<SelectionMode>(kSelectionModePropertyName);
    if (select_mode != nullptr) {
        return *select_mode;
    }
    else {
        return SelectionMode::Single;
    }
}

void ListControl::SetSelectionMode(SelectionMode selection_mode) {

    GetPropertyMap().SetProperty(kSelectionModePropertyName, selection_mode);
    
    item_container_->SetSelectStrategy(CreateSelectStrategy());

    if (selection_mode == SelectionMode::Single) {

        std::size_t first_selected_index = GetFirstSelectedItemIndex();
        if (first_selected_index != InvalidIndex) {
            ReplaceSelection(first_selected_index, 1);
            NotifySelectionChange();
        }
    }
    else if (selection_mode == SelectionMode::None) {
        UnselectAllItems();
    }
}


ListControl::SelectionChangeEvent::Proxy ListControl::GetSelectionChangeEvent() {
    return GetEventProxyFromPropertyMap<SelectionChangeEvent>(GetPropertyMap(), kSelectionChangeEventPropertyName);
}


const std::shared_ptr<internal::ListControlSelectStrategy> ListControl::CreateSelectStrategy() {

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

    select_strategy->SetListControl(std::dynamic_pointer_cast<ListControl>(shared_from_this()));
    select_strategy->SetItemHeightManager(item_height_manager_);
    return select_strategy;
}


void ListControl::ReplaceSelection(std::size_t index, std::size_t count) {

    item_selection_manager_->ReplaceSelection(index, count);

    for (std::size_t visible_item_index = 0; visible_item_index < visible_items_.size(); ++visible_item_index) {

        const auto& each_visible_item = visible_items_[visible_item_index];

        std::size_t item_index = first_visible_item_index_ + visible_item_index;
        bool is_selected = (index <= item_index) && (item_index < index + count);
        each_visible_item->SetIsSelected(is_selected);
    }
}


void ListControl::RevertSelection(std::size_t index) {

    item_selection_manager_->RevertSelection(index);

    if ((first_visible_item_index_ <= index) && (index < first_visible_item_index_ + visible_items_.size())) {

        const auto& visible_item = visible_items_[index - first_visible_item_index_];
        visible_item->SetIsSelected(! visible_item->IsSelected());
    }
}


void ListControl::AddSelection(std::size_t index, std::size_t count) {
    ChangeSelection(index, count, true);
}


void ListControl::RemoveSelection(std::size_t index, std::size_t count) {
    ChangeSelection(index, count, false);
}


void ListControl::ChangeSelection(std::size_t index, std::size_t count, bool is_add) {

    if (is_add) {
        item_selection_manager_->AddSelection(index, count);
    }
    else {
        item_selection_manager_->RemoveSelection(index, count);
    }

    std::size_t intersect_begin_index = std::max(index, first_visible_item_index_);
    std::size_t intersect_end_index = std::min(index + count, first_visible_item_index_ + visible_items_.size());
    if (intersect_begin_index >= intersect_end_index) {
        return;
    }

    for (std::size_t visible_item_index = intersect_begin_index - first_visible_item_index_;
         visible_item_index < intersect_end_index - first_visible_item_index_;
         ++visible_item_index) {

        visible_items_[visible_item_index]->SetIsSelected(is_add);
    }
}


void ListControl::SelectItemAtIndex(std::size_t index) {

    if ((index >= GetItemCount()) || IsItemSelectedAtIndex(index)) {
        return;
    }

    switch (GetSelectionMode()) {

        case SelectionMode::Single:
            ReplaceSelection(index, 1);
            break;

        case SelectionMode::SimpleMultiple:
        case SelectionMode::ExtendedMultiple:
            AddSelection(index, 1);
            break;

        default:
            return;
    }

    NotifySelectionChange();
}


void ListControl::UnselectItemAtIndex(std::size_t index) {

    if ((index >= GetItemCount()) || ! IsItemSelectedAtIndex(index)) {
        return;
    }

    RemoveSelection(index, 1);
    NotifySelectionChange();
}


void ListControl::SelectAllItems() {

    std::size_t item_count = GetItemCount();
    std::size_t selected_count = GetSelectedItemCount();
    if (selected_count != item_count) {
    
        ReplaceSelection(0, item_count);
        NotifySelectionChange();
    }
}


void ListControl::UnselectAllItems() {

    std::size_t selected_count = GetSelectedItemCount();
    if (selected_count != 0) {

        ReplaceSelection(0, 0);
        NotifySelectionChange();
    }
}


std::size_t ListControl::GetSelectedItemCount() const {
    return item_selection_manager_->GetAllSelectedCount();
}


std::vector<std::size_t> ListControl::GetSelectedItemIndexes() const {
    return item_selection_manager_->GetAllSelectedIndexes();
}


std::size_t ListControl::GetFirstSelectedItemIndex() const {
    return item_selection_manager_->GetFirstSelectedIndex();
}


bool ListControl::IsItemSelectedAtIndex(std::size_t index) const {
    return item_selection_manager_->IsIndexSelected(index);
}


void ListControl::NotifySelectionChange() {

    auto event = TryGetEventFromPropertyMap<SelectionChangeEvent>(GetPropertyMap(), kSelectionChangeEventPropertyName);
    if (event != nullptr) {
        event->Trigger(std::dynamic_pointer_cast<ListControl>(shared_from_this()));
    }
}


void ListControl::ScrollToItemAtIndex(std::size_t index) {

    auto position_and_height = item_height_manager_->GetItemPositionAndHeight(index);

    Rect visible_scroll_area_rect = GetVisibleScrollContentRect();
    if (position_and_height.first < visible_scroll_area_rect.position.y) {
        ScrollToScrollContentPosition(Point(0, position_and_height.first));
    }
    else {

        float end_position = position_and_height.first + position_and_height.second;
        if (end_position > visible_scroll_area_rect.position.y + visible_scroll_area_rect.size.height) {

            Point scroll_to_position;
            scroll_to_position.x = 0;
            scroll_to_position.y = end_position - visible_scroll_area_rect.size.height;
            ScrollToScrollContentPosition(scroll_to_position);
        }
    }
}


std::size_t ListControl::FindItemIndexAtPosition(const Point& position) const {

    auto visible_scroll_content_rect = GetVisibleScrollContentRect();

    if (position.x < 0 || position.x > visible_scroll_content_rect.size.width) {
        return InvalidIndex;
    }

    float adjusted_position = position.y + visible_scroll_content_rect.position.y;
    
    auto index_and_count = item_height_manager_->GetItemIndexAndCount(adjusted_position, adjusted_position);
    if (index_and_count.second == 0) {
        return InvalidIndex;
    }

    return index_and_count.first;
}


std::wstring ListControl::GetTypeName() const {
    return type::ListControl;
}


void ListControl::Item::Initialize() {

    __super::Initialize();

    SetBackgroundColorPicker([](const Control& control) {

        const auto& item = dynamic_cast<const Item&>(control);
        if (item.IsSelected()) {
            return Color::FromRGB(internal::ControlSelectedColorRGB);
        }
        else {
            return Color::Transparent;
        }
    });

    SetDefaultTextColorPicker([](const Control& control) {

        const auto& item = dynamic_cast<const Item&>(control);
        if (item.IsSelected()) {
            return Color::White;
        }
        else {
            return Color::FromRGB(internal::ControlNormalTextColorRGB);
        }
    });
}


ListControl::ItemContainer::ItemContainer() {

}


void ListControl::ItemContainer::Initialize() {

    __super::Initialize();

    SetBackgroundColor(Color::Transparent);
    SetCanFocused(true);
    SetLayouter(std::bind(&ItemContainer::LayoutItems, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}


void ListControl::ItemContainer::LayoutItems(
    const std::shared_ptr<Control>& parent,
    const Rect& previous_rect,
    const std::vector<std::shared_ptr<Control>>& children) {

    float width = GetContentSize().width;

    for (const auto& each_child : children) {

        auto child_rect = each_child->GetRect();
        if (child_rect.size.width == width) {
            continue;
        }

        child_rect.size.width = width;
        each_child->SetRect(child_rect);
    }
}


void ListControl::ItemContainer::MouseDown(const Point& position, const MouseMessage& message) {

    SetIsFocused(true);

    if (message.GetMouseButton() != MouseButton::Left) {
        __super::MouseDown(position, message);
        return;
    }

    CaptureMouse();
    select_strategy_->BeginChangingSelectionByMouseDown(position, message);
}


void ListControl::ItemContainer::MouseMove(const Point& position, const MouseMessage& message) {

    if (IsCapturingMouse()) {
        select_strategy_->ChangeSelectionByMouseMove(position, message);
    }
    else {
        __super::MouseMove(position, message);
    }
}


void ListControl::ItemContainer::MouseUp(const Point& position, const MouseMessage& message) {

    if (message.GetMouseButton() != MouseButton::Left) {
        __super::MouseUp(position, message);
        return;
    }

    if (IsCapturingMouse()) {
        ReleaseMouse();
    }

    select_strategy_->EndChangingSelectionByMouseUp(position, message);
}


bool ListControl::ItemContainer::KeyDown(const KeyMessage& message) {

    bool is_handled = select_strategy_->ChangeSelectionByKeyDown(message);
    if (is_handled) {
        return true;
    }
    else {
        return __super::KeyDown(message);
    }
}


static void CalculateRangeDifference(
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


void ListControl::ItemSource::NotifyItemAdd(std::size_t index, std::size_t count) {
    if (count != 0) {
        item_add_event_.Trigger(*this, index, count);
    }
}


void ListControl::ItemSource::NotifyItemRemove(std::size_t index, std::size_t count) {
    if (count != 0) {
        item_remove_event_.Trigger(*this, index, count);
    }
}


void ListControl::ItemSource::NotifyItemUpdate(std::size_t index, std::size_t count) {
    if (count != 0) {
        item_update_event_.Trigger(*this, index, count);
    }
}

}