#include <zaf/control/list_box.h>
#include <algorithm>
#include <zaf/base/assert.h>
#include <zaf/base/define.h>
#include <zaf/control/creation.h>
#include <zaf/window/message/key_message.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {

static const wchar_t* const kAllowMultiSelectPropertyName = L"AllowMultiSelect";
static const wchar_t* const kItemCreatorPropertyName = L"ItemCreator";
static const wchar_t* const kItemHeightPropertyName = L"ItemHeight";
static const wchar_t* const kSelectionChangeEventPropertyName = L"SelectionChangeEvent";

ListBox::ListBox() {

}


ListBox::~ListBox() {

}


void ListBox::Initialize() {
    
    __super::Initialize();

    SetBorderWidth(1);
    SetBorderColorPicker([](const Control&) { return Color::Black; });

    item_container_ = CreateControl<ItemContainer>(std::dynamic_pointer_cast<ListBox>(shared_from_this()));
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
            return CreateControl<Item>();
        };
    }
}

void ListBox::SetItemCreator(const ItemCreator& creator) {

    GetPropertyMap().SetProperty(kItemCreatorPropertyName, creator);
}


bool ListBox::AllowMultiSelect() const {

    auto allow = GetPropertyMap().TryGetProperty<bool>(kAllowMultiSelectPropertyName);
    if (allow != nullptr) {
        return *allow;
    }
    else {
        return false;
    }
}

void ListBox::SetAllowMultiSelect(bool allow) {

    GetPropertyMap().SetProperty(kAllowMultiSelectPropertyName, allow);

    if (! allow) {
        bool has_selection = false;
        for (const auto& each_item : items_) {

            if (each_item->IsSelected()) {
                if (has_selection) {
                    each_item->SetIsSelected(false);
                }
                else {
                    has_selection = true;
                }
            }
        }
    }
}


ListBox::SelectionChangeEvent::Proxy ListBox::GetSelectionChangeEvent() {

    auto& event = GetPropertyMap().GetProperty<SelectionChangeEvent>(kSelectionChangeEventPropertyName);
    return SelectionChangeEvent::Proxy(event);
}


std::size_t ListBox::AddItemWithTextAtIndex(const std::wstring& item_text, std::size_t index) {

    if (index == kInvalidIndex) {
        return kInvalidIndex;
    }

    auto item = GetItemCreator()(*this);
    if (item == nullptr) {
        ZAF_FAIL();
        return kInvalidIndex;
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
        return kInvalidIndex;
    }

    auto removed_item = *erase_iterator;

    std::size_t removed_index = std::distance(items_.begin(), erase_iterator);
    items_.erase(erase_iterator);

    RemoveItemFromContainer(removed_item);

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

    RemoveItemFromContainer(removed_item);
    return true;
}


void ListBox::RemoveItemFromContainer(const std::shared_ptr<Item>& item) {

    GetScrolledControl()->RemoveChild(item);
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
        return kInvalidIndex;
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


bool ListBox::SelectItemAtIndex(std::size_t index) {

    if (index >= items_.size()) {
        return false;
    }

    auto item = items_[index];
    SelectItem(item);
    ScrollToItem(item);
    SelectionChange();
    return true;
}


void ListBox::SelectItem(const std::shared_ptr<Item>& item) {

    for (const auto& each_item : items_) {
        if (each_item->IsSelected() && (each_item != item)) {
            each_item->SetIsSelected(false);
        }
    }

    item->SetIsSelected(true);
}


void ListBox::SelectionChange() {

    auto event = GetPropertyMap().TryGetProperty<SelectionChangeEvent>(kSelectionChangeEventPropertyName);
    if (event != nullptr) {
        event->Trigger(std::dynamic_pointer_cast<ListBox>(shared_from_this()));
    }
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


void ListBox::Item::Initialize() {

    SetBackgroundColorPicker([](const Control& control) {
    
        const auto& item = dynamic_cast<const Item&>(control);
        if (item.IsSelected()) {
            return Color::Blue;
        }
        else {
            return Color::White;
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


void ListBox::ItemContainer::MouseDown(const Point& position, const MouseMessage& message) {

    SetIsFocused(true);

    if (message.button != MouseButton::Left) {
        return;
    }

    NeedCaptureMouse(true);
    SelectItemAtPosition(position);
}


void ListBox::ItemContainer::MouseUp(const Point& position, const MouseMessage& message) {

    if (message.button != MouseButton::Left) {
        return;
    }

    NeedCaptureMouse(false);

    if (selecting_item_ != nullptr) {

        auto list_box = list_box_.lock();
        if (list_box != nullptr) {
            list_box->SelectionChange();
        }
    }

    selecting_item_ = nullptr;
}


void ListBox::ItemContainer::MouseMove(const Point& position, const MouseMessage& message) {

    if (IsCapturingMouse()) {
        SelectItemAtPosition(position);
    }
}


void ListBox::ItemContainer::KeyDown(const KeyMessage& message) {

    auto list_box = list_box_.lock();
    if (list_box == nullptr) {
        return;
    }

    if (list_box->AllowMultiSelect()) {
        return;
    }

    std::size_t index = list_box->GetFirstSelectedItemIndex();

    switch (message.wParam) {
        case VK_DOWN:
            if (index == kInvalidIndex) {
                index = 0;
            }
            else {
                ++index;
            }
            break;
    
        case VK_UP:
            if (index == kInvalidIndex) {
                index = list_box->items_.size() - 1;
            }
            else {
                --index;
            }
            break;
    
        case VK_HOME:
            index = 0;
            break;

        case VK_END:
            index = list_box->items_.size() - 1;
            break;

        default:
            return;
    }
    
    list_box->SelectItemAtIndex(index);
}


void ListBox::ItemContainer::SelectItemAtPosition(const Point& position) {

    auto child = FindChildAtPosition(position);
    auto item = std::dynamic_pointer_cast<Item>(child);
    if (item == nullptr) {
        return;
    }

    auto list_box = list_box_.lock();
    if (list_box != nullptr) {
        list_box->SelectItem(item);
    }

    selecting_item_ = item;
}

}