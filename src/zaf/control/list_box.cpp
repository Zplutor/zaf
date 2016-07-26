#include <zaf/control/list_box.h>
#include <algorithm>
#include <zaf/base/assert.h>
#include <zaf/base/event_utility.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>
#include <zaf/window/message/key_message.h>
#include <zaf/window/message/mouse_message.h>

#undef max
#undef min

namespace zaf {

static const wchar_t* const kItemCreatorPropertyName = L"ItemCreator";
static const wchar_t* const kItemHeightPropertyName = L"ItemHeight";
static const wchar_t* const kSelectionChangeEventPropertyName = L"SelectionChangeEvent";
static const wchar_t* const kSelectOptionPropertyName = L"SelectOption";

ListBox::ListBox() {

}


ListBox::~ListBox() {

}


void ListBox::Initialize() {
    
    __super::Initialize();

    SetBorderThickness(1);
    SetBorderColorPicker([](const Control&) { return Color::Black; });

    item_container_ = Create<ItemContainer>(std::dynamic_pointer_cast<ListBox>(shared_from_this()));
    item_container_->SetBackgroundColorPicker([](const Control&) { 
        return Color::White;
    });
    SetScrolledControl(item_container_);
}


void ListBox::UpdateScrollAreaSize() {

    float item_height = GetItemHeight();
    Size scroll_area_size(0, item_height * items_.size());
    SetScrollAreaSize(scroll_area_size);
}


float ListBox::GetItemHeight() const {

    auto item_height = GetPropertyMap().TryGetProperty<float>(kItemHeightPropertyName);
    if (item_height != nullptr) {
        return *item_height;
    }
    else {
        return 18;
    }
}

void ListBox::SetItemHeight(float item_height) {

    GetPropertyMap().SetProperty(kItemHeightPropertyName, item_height);
    UpdateScrollAreaSize();
}


const ListBox::ItemCreator ListBox::GetItemCreator() const {

    auto item_creator = GetPropertyMap().TryGetProperty<ItemCreator>(kItemCreatorPropertyName);
    if (item_creator != nullptr) {
        return *item_creator;
    }
    else {
        return [](const ListBox&) {
            return Create<Item>();
        };
    }
}

void ListBox::SetItemCreator(const ItemCreator& creator) {

    GetPropertyMap().SetProperty(kItemCreatorPropertyName, creator);
}


ListBox::SelectOption ListBox::GetSelectOption() const {

    auto select_option = GetPropertyMap().TryGetProperty<SelectOption>(kSelectOptionPropertyName);
    if (select_option != nullptr) {
        return *select_option;
    }
    else {
        return SelectOption::SingleSelect;
    }
}


void ListBox::SetSelectOption(SelectOption select_option) {

    GetPropertyMap().SetProperty(kSelectOptionPropertyName, select_option);

    if (select_option == SelectOption::SingleSelect) {
        SelectItemAtIndex(GetFirstSelectedItemIndex());
    }
    else if (select_option == SelectOption::NoSelect) {
        UnselectAllItems();
    }
}


ListBox::SelectionChangeEvent::Proxy ListBox::GetSelectionChangeEvent() {
    return GetEventProxyFromPropertyMap<SelectionChangeEvent>(GetPropertyMap(), kSelectionChangeEventPropertyName);
}


std::size_t ListBox::AddItemWithTextAtIndex(const std::wstring& item_text, std::size_t index) {

    if (index == InvalidIndex) {
        return InvalidIndex;
    }

    auto item = GetItemCreator()(*this);
    if (item == nullptr) {
        ZAF_FAIL();
        return InvalidIndex;
    }

    item->SetText(item_text);

    std::size_t revised_index = index <= items_.size() ? index : items_.size();

    auto insert_iterator = items_.begin();
    std::advance(insert_iterator, revised_index);
    items_.insert(insert_iterator, item);

    GetScrolledControl()->AddChild(item);
    UpdateScrollAreaSize();

    return revised_index;
}


std::size_t ListBox::RemoveItemWithText(const std::wstring& item_text) {

    auto erase_iterator = std::find_if(
        items_.begin(), 
        items_.end(),
        [&item_text](const std::shared_ptr<Item>& item) {
            return item->GetText() == item_text;
        }
    );

    if (erase_iterator == items_.end()) {
        return InvalidIndex;
    }

    auto removed_item = *erase_iterator;

    std::size_t removed_index = std::distance(items_.begin(), erase_iterator);
    items_.erase(erase_iterator);

    RemoveItem(removed_item);
    return removed_index;
}


bool ListBox::RemoveItemAtIndex(std::size_t index) {

    if (index >= items_.size()) {
        return false;
    }

    auto erase_iterator = items_.begin();
    std::advance(erase_iterator, index);

    auto removed_item = *erase_iterator;

    items_.erase(erase_iterator);

    RemoveItem(removed_item);
    return true;
}


void ListBox::RemoveItem(const std::shared_ptr<Item>& item) {

    item_container_->RemoveItem(item);
    UpdateScrollAreaSize();

    if (item->IsSelected()) {
        SelectionChange();
    }
}


const std::wstring ListBox::GetItemTextAtIndex(std::size_t index) {

    if (index >= items_.size()) {
        return std::wstring();
    }

    return items_[index]->GetText();
}


std::size_t ListBox::GetFirstSelectedItemIndex() const {

    auto iterator = std::find_if(
        items_.begin(), 
        items_.end(), 
        [](const std::shared_ptr<Item>& item) {
            return item->IsSelected();
        }
    );

    if (iterator == items_.end()) {
        return InvalidIndex;
    }

    return std::distance(items_.begin(), iterator);
}


const std::wstring ListBox::GetFirstSelectedItemText() const {

    for (const auto& each_item : items_) {
        if (each_item->IsSelected()) {
            return each_item->GetText();
        }
    }

    return std::wstring();
}


const std::vector<std::size_t> ListBox::GetSelectedItemIndexes() const {

    std::vector<std::size_t> indexes;

    for (std::size_t index = 0; index < items_.size(); ++index) {
        if (items_[index]->IsSelected()) {
            indexes.push_back(index);
        }
    }

    return indexes;
}


const std::vector<std::wstring> ListBox::GetSelectedItemTexts() const {

    std::vector<std::wstring> texts;

    for (const auto& each_item : items_) {
        if (each_item->IsSelected()) {
            texts.push_back(each_item->GetText());
        }
    }

    return texts;
}


bool ListBox::SelectAllItems() {
    
    auto select_option = GetSelectOption();
    if (select_option == SelectOption::NoSelect || select_option == SelectOption::SingleSelect) {
        return false;
    }

    bool has_changed_selection = false;
    for (const auto& each_item : items_) {

        if (! each_item->IsSelected()) {
            each_item->SetIsSelected(true);
            has_changed_selection = true;
        }
    }

    if (has_changed_selection) {
        SelectionChange();
    }
    return true;
}


void ListBox::UnselectAllItems() {

    bool has_changed_selection = false;
    for (const auto& each_item : items_) {
        
        if (each_item->IsSelected()) {
            each_item->SetIsSelected(false);
            has_changed_selection = true;
        }
    }

    if (has_changed_selection) {
        SelectionChange();
    }
}


bool ListBox::SelectItemAtIndex(std::size_t index) {

    if (index >= items_.size()) {
        return false;
    }

    auto item = items_[index];
    SelectItem(item, ! CanMultiSelect(), true);
    SelectionChange();
    return true;
}


void ListBox::SelectItem(const std::shared_ptr<Item>& item, bool unselect_others, bool scroll_to_it) {

    if (unselect_others) {
        for (const auto& each_item : items_) {
            if (each_item->IsSelected() && (each_item != item)) {
                each_item->SetIsSelected(false);
            }
        }
    }

    item->SetIsSelected(true);

    if (scroll_to_it) {
        ScrollToItem(item);
    }
}


void ListBox::UnselectItem(const std::shared_ptr<Item>& item, bool scroll_to_it) {

    item->SetIsSelected(false);

    if (scroll_to_it) {
        ScrollToItem(item);
    }
}


bool ListBox::ScrollToItemAtIndex(std::size_t index) {

    if (index >= items_.size()) {
        return false;
    }

    auto item = items_[index];
    ScrollToItem(item);
    return true;
}


void ListBox::ScrollToItem(const std::shared_ptr<Item>& item) {

    Rect visible_scroll_area_rect = GetVisibleScrollAreaRect();
    Rect item_rect = item->GetRect();

    if (item_rect.position.y < visible_scroll_area_rect.position.y) {
        ScrollToScrollAreaPosition(item_rect.position);
    }
    else if (item_rect.position.y + item_rect.size.height >
             visible_scroll_area_rect.position.y + visible_scroll_area_rect.size.height) {

        Point scroll_to_position;
        scroll_to_position.x = 0;
        scroll_to_position.y = item_rect.position.y + item_rect.size.height - visible_scroll_area_rect.size.height;
        ScrollToScrollAreaPosition(scroll_to_position);
    }
}


std::size_t ListBox::GetItemIndex(const std::shared_ptr<Item>& item) const {

    auto iterator = std::find(items_.begin(), items_.end(), item);
    if (iterator == items_.end()) {
        return InvalidIndex;
    }
    else {
        return std::distance(items_.begin(), iterator);
    }
}


void ListBox::SelectionChange() {

    auto event = TryGetEventFromPropertyMap<SelectionChangeEvent>(GetPropertyMap(), kSelectionChangeEventPropertyName);
    if (event != nullptr) {
        event->Trigger(std::dynamic_pointer_cast<ListBox>(shared_from_this()));
    }
}


void ListBox::Item::Initialize() {

    SetBackgroundColorPicker([](const Control& control) {
    
        const auto& item = dynamic_cast<const Item&>(control);
        if (item.IsSelected()) {
            return Color::FromRGB(internal::ControlSelectedColorRGB);
        }
        else {
            return Color::Transparent;
        }
    });

    SetTextColorPicker([](const Control& control) { 
    
        const auto& item = dynamic_cast<const Item&>(control);
        if (item.IsSelected()) {
            return Color::White;
        }
        else {
            return Color::Black;
        }
    });
}


void ListBox::ItemContainer::Initialize() {

    SetCanFocused(true);
    SetLayouter(std::bind(
        &ListBox::ItemContainer::LayoutItems,
        this, 
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3)
    );
}


void ListBox::ItemContainer::LayoutItems(
    const std::shared_ptr<Control>&,
    const Rect& preivous_rect,
    const std::vector<std::shared_ptr<Control>>& children
) {

    auto list_box = list_box_.lock();
    if (list_box == nullptr) {
        return;
    }

    float width = this->GetWidth();
    float height = list_box->GetItemHeight();
    Rect item_rect(0, 0, width, height);

    for (const auto& each_item : list_box->items_) {
        each_item->SetRect(item_rect);
        item_rect.position.y += height;
    }
}


void ListBox::ItemContainer::RemoveItem(const std::shared_ptr<Item>& item) {

    RemoveChild(item);

    if (last_item_ == item) {
        UpdateLastItem(nullptr);
    }

    if (last_item_with_shift_key_ == item) {
        last_item_with_shift_key_ = last_item_;
    }

    if (mouse_down_item_ == item) {
        mouse_down_item_ = nullptr;
        is_mouse_down_item_originally_selected_ = false;
    }

    originally_selected_items_.erase(item);
}


void ListBox::ItemContainer::MouseDown(const Point& position, const MouseMessage& message) {

    SetIsFocused(true);

    if (message.GetMouseButton() != MouseButton::Left) {
        return;
    }

    NeedCaptureMouse(true);
    is_changing_selection_ = SelectItemAtPositionByMouseEvent(position, false); 
}


void ListBox::ItemContainer::MouseMove(const Point& position, const MouseMessage& message) {

    if (IsCapturingMouse()) {
        SelectItemAtPositionByMouseEvent(position, true);
    }
}


void ListBox::ItemContainer::MouseUp(const Point& position, const MouseMessage& message) {

    if (message.GetMouseButton() != MouseButton::Left) {
        return;
    }

    if (IsCapturingMouse()) {
        NeedCaptureMouse(false);
    }

    if (is_changing_selection_) {

        auto list_box = list_box_.lock();
        if (list_box != nullptr) {
            list_box->SelectionChange();
        }
    }

    is_changing_selection_ = false;
    mouse_down_item_ = nullptr;
    is_mouse_down_item_originally_selected_ = false;
    originally_selected_items_.clear();
}


bool ListBox::ItemContainer::SelectItemAtPositionByMouseEvent(const Point& position, bool is_mouse_moving) {

    auto child = FindChildAtPosition(position);
    auto item = std::dynamic_pointer_cast<Item>(child);
    if (item == nullptr) {
        return false;
    }

    auto list_box = list_box_.lock();
    if (list_box == nullptr) {
        return false;
    }

    switch (list_box->GetSelectOption()) {

        case SelectOption::SingleSelect:
            SingleSelectItemByMouseEvent(list_box, item);
            break;

        case SelectOption::SimpleMultiSelect:
            SimpleMultiSelectItemByMouseEvent(list_box, item, is_mouse_moving);
            break;

        case SelectOption::ExtendedMultiSelect:
            ExtendedMultiSelectItemByMouseEvent(list_box, item, is_mouse_moving);
            break;

        case SelectOption::NoSelect:
            return false;

        default:
            ZAF_FAIL();
            return false;
    }

    return true;
}


void ListBox::ItemContainer::SingleSelectItemByMouseEvent(const std::shared_ptr<ListBox>& list_box, const std::shared_ptr<Item>& item) {
    list_box->SelectItem(item, true, true);
}


void ListBox::ItemContainer::SimpleMultiSelectItemByMouseEvent(
    const std::shared_ptr<ListBox>& list_box,
    const std::shared_ptr<Item>& item,
    bool is_mouse_moving) {

    if (is_mouse_moving) {
        return;
    }

    if (item->IsSelected()) {
        list_box->UnselectItem(item, true);
    }
    else {
        list_box->SelectItem(item, false, true);
    }
}


void ListBox::ItemContainer::ExtendedMultiSelectItemByMouseEvent(
    const std::shared_ptr<ListBox>& list_box,
    const std::shared_ptr<Item>& item, 
    bool is_mouse_moving
) {

    bool is_pressing_shift_key = GetKeyState(VK_SHIFT) < 0;
    bool is_pressing_control_key = GetKeyState(VK_CONTROL) < 0;

    if (is_pressing_shift_key && is_pressing_control_key) {
        return;
    }

    if (is_pressing_shift_key) {
        ExtendedMultiSelectItemWithShiftKey(list_box, item, is_mouse_moving);
    }
    else if (is_pressing_control_key) {
        ExtendedMultiSelectItemWithControlKey(list_box, item, is_mouse_moving);
    }
    else {
        ExtendedMultiSelectItemWithMouseOnly(list_box, item, is_mouse_moving);
    }
}


void ListBox::ItemContainer::ExtendedMultiSelectItemWithMouseOnly(
    const std::shared_ptr<ListBox>& list_box, 
    const std::shared_ptr<Item>& item,
    bool is_mouse_moving
) {

    if (! is_mouse_moving) {
        UpdateLastItem(item);
        mouse_down_item_ = item;
    }

    ChangeSelectionBetweenItems(list_box, mouse_down_item_, item, true, false);
}


void ListBox::ItemContainer::ExtendedMultiSelectItemWithShiftKey(
    const std::shared_ptr<ListBox>& list_box,
    const std::shared_ptr<Item>& item,
    bool is_mouse_moving
) {

    if (! is_mouse_moving) {
        mouse_down_item_ = last_item_;
    }

    ChangeSelectionBetweenItems(list_box, mouse_down_item_, item, true, false);
}


void ListBox::ItemContainer::ExtendedMultiSelectItemWithControlKey(
    const std::shared_ptr<ListBox>& list_box,
    const std::shared_ptr<Item>& item,
    bool is_mouse_moving
) {

    if (! is_mouse_moving) {

        UpdateLastItem(item);

        mouse_down_item_ = item;
        is_mouse_down_item_originally_selected_ = mouse_down_item_->IsSelected();

        //Record all selected items.
        originally_selected_items_.clear();
        for (const auto& each_item : list_box->items_) {
            if (each_item->IsSelected()) {
                originally_selected_items_.insert(each_item);
            }
        }
    }

    ChangeSelectionBetweenItems(list_box, mouse_down_item_, item, ! is_mouse_down_item_originally_selected_, true);
}


void ListBox::ItemContainer::ChangeSelectionBetweenItems(
    const std::shared_ptr<ListBox>& list_box,
    const std::shared_ptr<Item>& begin_item,
    const std::shared_ptr<Item>& end_item,
    bool is_selecting,
    bool only_change_items_in_range
) {

    std::size_t range_bound_count = begin_item == end_item ? 1 : 2;
    bool is_in_range = false;

    for (const auto& each_item : list_box->items_) {

        if (each_item == begin_item || each_item == end_item) {
            --range_bound_count;
            is_in_range = true;
        }
        else if (range_bound_count <= 0) {
            is_in_range = false;
        }
         
        bool should_select = false;;

        //Recover select state for those items not in range.
        if (! is_in_range && only_change_items_in_range) {
            
            auto iterator = originally_selected_items_.find(each_item);
            if (iterator == originally_selected_items_.end()) {
                should_select = false;
            }
            else {
                should_select = true;
            }
        }
        else {
            should_select = (is_in_range && is_selecting) || (! is_in_range && ! is_selecting);
        }

        if (should_select) {
            list_box->SelectItem(each_item, false, false);
        }
        else {
            list_box->UnselectItem(each_item, false);
        }
    }

    list_box->ScrollToItem(end_item);
}


bool ListBox::ItemContainer::KeyDown(const KeyMessage& message) {

    bool is_handled = false;

    auto list_box = list_box_.lock();
    if (list_box != nullptr) {
        
        SelectOption select_option = list_box->GetSelectOption();
        switch (select_option) {

            case SelectOption::SingleSelect:
                is_handled = SingleSelectItemByKeyEvent(list_box, message);
                break;
        
            case SelectOption::ExtendedMultiSelect:
                is_handled = ExtendedMultiSelectItemByKeyEvent(list_box, message);
                break;

            case SelectOption::SimpleMultiSelect:
            case SelectOption::NoSelect:
                break;

            default:
                ZAF_FAIL();
                break;
        }
    }

    if (is_handled) {
        return true;
    }
    else {
        return __super::KeyDown(message);
    }
}


bool ListBox::ItemContainer::SingleSelectItemByKeyEvent(const std::shared_ptr<ListBox>& list_box, const KeyMessage& key_message) {

    std::size_t previous_index = list_box->GetFirstSelectedItemIndex();
    std::size_t new_index = 0;
    bool is_changed = ChangeIndexWithKeyMessage(list_box, previous_index, key_message, new_index);
    if (! is_changed) {
        return false;
    }

    if (new_index != InvalidIndex) {
        list_box->SelectItemAtIndex(new_index);
    }
    return true;
}


bool ListBox::ItemContainer::ExtendedMultiSelectItemByKeyEvent(const std::shared_ptr<ListBox>& list_box, const KeyMessage& key_message) {

    bool is_pressing_shift_key = (GetKeyState(VK_SHIFT) < 0);

    std::size_t previous_index = InvalidIndex;
    if (is_pressing_shift_key) {
        previous_index = list_box->GetItemIndex(last_item_with_shift_key_);
    }

    if (previous_index == InvalidIndex) {
        previous_index = list_box->GetItemIndex(last_item_);
    }

    std::size_t new_index = 0;
    bool is_changed = ChangeIndexWithKeyMessage(list_box, previous_index, key_message, new_index);
    if (! is_changed) {
        return false;
    }

    std::shared_ptr<Item> begin_select_item;
    auto end_select_item = list_box->items_[new_index];

    if (is_pressing_shift_key) {

        begin_select_item = last_item_;
        last_item_with_shift_key_ = end_select_item;
    }
    else {

        begin_select_item = end_select_item;

        //Update last item to the new selected item.
        UpdateLastItem(end_select_item);
    }

    ChangeSelectionBetweenItems(list_box, begin_select_item, end_select_item, true, false);
    return true;
}


bool ListBox::ItemContainer::ChangeIndexWithKeyMessage(
    const std::shared_ptr<ListBox>& list_box,
    std::size_t previous_index, 
    const KeyMessage& message,
    std::size_t& new_index
) const {

    new_index = previous_index;

    switch (message.wParam) {
        case VK_DOWN:
            if (new_index == InvalidIndex) {
                new_index = 0;
            }
            else if (new_index < list_box->items_.size() - 1) {
                ++new_index;
            }
            return true;

        case VK_UP:
            if (new_index == InvalidIndex) {
                new_index = list_box->items_.size() - 1;
            }
            else if (new_index > 0) {
                --new_index;
            }
            return true;

        case VK_HOME:
            new_index = 0;
            return true;

        case VK_END:
            new_index = list_box->items_.size() - 1;
            return true;

        default:
            return false;
    }
}

}