#include <zaf/control/list_box.h>
#include <zaf/creation.h>

namespace zaf {

ListBox::ListBox() {

}


ListBox::~ListBox() {

}


void ListBox::Initialize() {
    
    __super::Initialize();

    SetItemSource(std::make_shared<ItemSource>());
}


void ListBox::ItemSourceChange(const std::shared_ptr<ListControl::ItemSource>& previous_item_source) {

    __super::ItemSourceChange(previous_item_source);
    item_source_ = std::dynamic_pointer_cast<ItemSource>(GetItemSource());
}


float ListBox::GetItemHeight() const {

    if (item_source_ != nullptr) {
        return item_source_->GetItemHeight(0);
    }
    else {
        return 0;
    }
}


void ListBox::SetItemHeight(float item_height) {

    if (item_source_ != nullptr) {
        item_source_->SetItemHeight(item_height);
        Reload();
    }
}


std::size_t ListBox::AddItemWithTextAtIndex(const std::wstring& item_text, std::size_t index) {

    if (item_source_ != nullptr) {
        return item_source_->AddItemWithTextAtIndex(item_text, index);
    }
    else {
        return InvalidIndex;
    }
}


std::size_t ListBox::RemoveItemWithText(const std::wstring& item_text) {

    if (item_source_ != nullptr) {
        return item_source_->RemoveItemWithText(item_text);
    }
    else {
        return InvalidIndex;
    }
}


bool ListBox::RemoveItemAtIndex(std::size_t index) {

    if (item_source_ != nullptr) {
        return item_source_->RemoveItemAtIndex(index);
    }
    else {
        return false;
    }
}


const std::wstring ListBox::GetItemTextAtIndex(std::size_t index) {

    if (item_source_ != nullptr) {
        return item_source_->GetItemTextAtIndex(index);
    }
    else {
        return std::wstring();
    }
}


const std::wstring ListBox::GetFirstSelectedItemText() const {

    if (item_source_ != nullptr) {
        std::size_t first_selected_index = GetFirstSelectedItemIndex();
        return item_source_->GetItemTextAtIndex(first_selected_index);
    }
    else {
        return std::wstring();
    }
}


const std::vector<std::wstring> ListBox::GetSelectedItemTexts() const {

    if (item_source_ != nullptr) {

        auto selected_indexes = GetSelectedItemIndexes();

        std::vector<std::wstring> texts;
        texts.reserve(selected_indexes.size());

        for (auto each_index : selected_indexes) {
            texts.push_back(item_source_->GetItemTextAtIndex(each_index));
        }

        return texts;
    }
    else {

        return std::vector<std::wstring>();
    }
}


void ListBox::ItemSource::LoadItem(std::size_t index, const std::shared_ptr<Item>& item) {
    item->SetText(item_texts_[index]);
}


std::size_t ListBox::ItemSource::AddItemWithTextAtIndex(const std::wstring& text, std::size_t index) {

    if (index == InvalidIndex) {
        return InvalidIndex;
    }

    std::size_t revised_index = index <= item_texts_.size() ? index : item_texts_.size();
    item_texts_.insert(std::next(item_texts_.begin(), index), text);

    NotifyItemAdd(index, 1);
    return revised_index;
}


std::size_t ListBox::ItemSource::RemoveItemWithText(const std::wstring& item_text) {

    auto erase_iterator = std::find(item_texts_.begin(), item_texts_.end(), item_text);
    if (erase_iterator == item_texts_.end()) {
        return InvalidIndex;
    }

    std::size_t removed_index = std::distance(item_texts_.begin(), erase_iterator);
    item_texts_.erase(erase_iterator);

    NotifyItemRemove(removed_index, 1);
    return removed_index;
}


bool ListBox::ItemSource::RemoveItemAtIndex(std::size_t index) {

    if (index >= item_texts_.size()) {
        return false;
    }

    item_texts_.erase(std::next(item_texts_.begin(), index));

    NotifyItemRemove(index, 1);
    return true;
}


const std::wstring ListBox::ItemSource::GetItemTextAtIndex(std::size_t index) {

    if (index >= item_texts_.size()) {
        return std::wstring();
    }

    return item_texts_[index];
}


}