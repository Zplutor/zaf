#pragma once

#include <zaf/control/list_control.h>

namespace zaf {

/**
 Represents a list box control.   
 */
class ListBox : public ListControl {
public:
    class ItemSource : public ListControl::ItemSource {
    public:
        ItemSource() : item_height_(18) {
        
        }

        std::size_t GetItemCount() override {
            return item_texts_.size();
        }

        float GetItemHeight(std::size_t index) override {
            return item_height_;
        }

        void LoadItem(std::size_t index, const std::shared_ptr<Item>& item) override;

    private:
        friend class ListBox;

        void SetItemHeight(float item_height) {
            item_height_ = item_height;
        }

        std::size_t AddItemWithTextAtIndex(const std::wstring& text, std::size_t index);
        std::size_t RemoveItemWithText(const std::wstring& item_text);
        bool RemoveItemAtIndex(std::size_t index);
        const std::wstring GetItemTextAtIndex(std::size_t index);

    private:
        float item_height_;
        std::vector<std::wstring> item_texts_;
    };

public:
    ListBox();
    ~ListBox();

    void Initialize() override;

    /**
     Get item height.

     The default height is 18.
     */
    float GetItemHeight() const;

    /**
     Set item height. 
     */
    void SetItemHeight(float item_height);

    /**
     Add a new item with specified text to the end of list box.

     @param item_text
        The text of new item.

     @return
        Return the index of the newly add item, or InvalidIndex if the operation fails.
     */
    std::size_t AddItemWithText(const std::wstring& item_text) {
        return AddItemWithTextAtIndex(item_text, GetItemCount());
    }

    /**
     Add a new item with specified text to specified index.

     @param item_text
        The text of new item.

     @param index
        The index where to add. Can not be InvalidIndex.

     @return 
        Return the index of the newly add item, or InvalidIndex if the operation fails.

     If index is greator then the count of items, a new item is added to the end.
     */
    std::size_t AddItemWithTextAtIndex(const std::wstring& item_text, std::size_t index);

    /**
     Remove the first item that matches specified text.

     @param item_text
        The text to be matched.

     @return 
        Return index of the removed item, or InvalidIndex if no item is matched.
     */
    std::size_t RemoveItemWithText(const std::wstring& item_text);

    /**
     Remove the item at specified index.

     @param index
        The index of item to be removed.

     @return 
        Return an value indicating that whether the operation succeeds.
     */
    bool RemoveItemAtIndex(std::size_t index);

    /**
     Get the text of item at specified index.

     @param index
        The index of item.

     @return 
        Return the text of item, or empty string if the item is inexistent.
     */
    const std::wstring GetItemTextAtIndex(std::size_t index);

    /**
     Get text of the first selected item.

     @return
        Return the text, or empty string if no item is selected.
     */
    const std::wstring GetFirstSelectedItemText() const;

    /**
     Get a text list of all selected items.
     */
    const std::vector<std::wstring> GetSelectedItemTexts() const;

protected:
    void ItemSourceChange(const std::shared_ptr<ListControl::ItemSource>& previous_item_source) override;

private:
    std::shared_ptr<ItemSource> item_source_;
};

}