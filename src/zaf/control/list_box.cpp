#include <zaf/control/list_box.h>
#include <algorithm>
#include <zaf/base/assert.h>
#include <zaf/base/define.h>

namespace zaf {

static const wchar_t* const kItemCreatorPropertyName = L"ItemCreator";
static const wchar_t* const kItemHeightPropertyName = L"ItemHeight";

ListBox::ListBox() {

}


ListBox::~ListBox() {

}


void ListBox::Initialize() {
    
    __super::Initialize();

    SetBorderWidth(1);
    SetBorderColorPicker([](const Control&) { return Color::Black; });

    GetScrolledControl()->SetLayouter(std::bind(
        &ListBox::ItemLayouter, 
        this,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));
}


void ListBox::ItemLayouter(
    const std::shared_ptr<Control>& scrolled_control,
    const Rect& preivous_rect,
    const std::vector<std::shared_ptr<Control>>& children
) {

    float width = scrolled_control->GetWidth();
    float height = GetItemHeight();
    Rect item_rect(0, 0, width, height);
    
    for (const auto& each_item : items_) {
        each_item->SetRect(item_rect);
        item_rect.position.y += height;
    }
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
            return std::make_shared<Item>();
        };
    }
}

void ListBox::SetItemCreator(const ItemCreator& creator) {

    GetPropertyMap().SetProperty(kItemCreatorPropertyName, creator);
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

    RemoveItemFromScrolledControl(removed_item);

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

    RemoveItemFromScrolledControl(removed_item);
    return true;
}


void ListBox::RemoveItemFromScrolledControl(const std::shared_ptr<Item>& item) {

    GetScrolledControl()->RemoveChild(item);
    UpdateScrollAreaSize();
}


const std::wstring ListBox::GetItemTextAtIndex(std::size_t index) {

    if (index >= items_.size()) {
        return std::wstring();
    }

    return items_[index]->GetText();
}

}