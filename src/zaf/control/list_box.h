#pragma once

#include <functional>
#include <zaf/base/define.h>
#include <zaf/base/event.h>
#include <zaf/control/scrollable_control.h>
#include <zaf/control/textual_control.h>

namespace zaf {

/**
 Represents a list box control.   
 */
class ListBox : public ScrollableControl {
public:
    /**
     Represents a list box item.
     */
    class Item : public TextualControl {
    public:
        Item() : is_selected_(false) { }

        void Initialize() override;

        /**
         Get a value indicating that whether the item is selected.
         */
        bool IsSelected() const {
            return is_selected_;
        }

    private:
        friend class ListBox;

        void SetIsSelected(bool is_selected) {
            if (is_selected_ != is_selected) {
                is_selected_ = is_selected;
                NeedRepaint();
            }
        }

    private:
        bool is_selected_;
    };

    /**
     Defines the selection mode of a list box.
     */
    enum class SelectionMode {

        /**
         The list box items can not be selected.
         */
        None,

        /**
         Only one single list box item can be selected.
         */
        Single,

        /**
         Multiple list box items can be selected with mouse clicking.
         */
        SimpleMultiple,

        /**
         Multiple list box items can be selected with the help of SHIFT and CTRL keys.
         */
        ExtendedMultiple,
    };

    /**
     The prototype of item creator.

     @param list_box
        The list box that creating the item.

     @return 
        The created item. Must no be nullptr.

     List box use item creator to create items.
     */
    typedef std::function<const std::shared_ptr<Item>(const ListBox& list_box)> ItemCreator;

    /**
     The type of selection change event.
     */
    typedef Event<const std::shared_ptr<ListBox>&> SelectionChangeEvent;

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
     Get item creator.

     The default item creator creates instances of ListBox::Item.
     */
    const ItemCreator GetItemCreator() const;

    /**
     Set item creator.
     */
    void SetItemCreator(const ItemCreator& creator);

    /**
     Get selection mode.

     The default selection mode is Single.
     */
    SelectionMode GetSelectionMode() const;

    /**
     Set seletion mode.

     All selected items would be unselected if selection mode is set 
     to None; and only the first selected item would remain selected 
     if selection mode is set to Single.
     */
    void SetSelectionMode(SelectionMode selection_mode);

    /**
     Get selection change event.

     This event is raised when the selection is changed.
     */
    SelectionChangeEvent::Proxy GetSelectionChangeEvent();

    /**
     Add a new item with specified text to the end of list box.

     @param item_text
        The text of new item.

     @return
        Return the index of the newly add item, or InvalidIndex if the operation fails.
     */
    std::size_t AddItemWithText(const std::wstring& item_text) {
        return AddItemWithTextAtIndex(item_text, items_.size());
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
     Get the count of all items.
     */
    std::size_t GetItemCount() const {
        return items_.size();
    }

    /**
     Get the text of item at specified index.

     @param index
        The index of item.

     @return 
        Return the text of item, or empty string if the item is inexistent.
     */
    const std::wstring GetItemTextAtIndex(std::size_t index);

    /**
     Get index of the first selected item.

     @return 
        Return the index, or InvalidIndex if no item is selected.
     */
    std::size_t GetFirstSelectedItemIndex() const;

    /**
     Get text of the first selected item.

     @return
        Return the text, or empty string if no item is selected.
     */
    const std::wstring GetFirstSelectedItemText() const;

    /**
     Get an index list of all selected items.
     */
    const std::vector<std::size_t> GetSelectedItemIndexes() const;

    /**
     Get a text list of all selected items.
     */
    const std::vector<std::wstring> GetSelectedItemTexts() const;

    /**
     Select all items.

     @return 
        Return an value indicating that whether the operation succeeds.

     This methods succeeds only when the selection mode is SimpleMultiple or ExtendedMultiple.
     */
    bool SelectAllItems();

    /**
     Unselect all items. 
     */
    void UnselectAllItems();

    /**
     Select an item at specified index.

     @param index
        The index of item to be selected.
       
     @return 
        Return an value indicating that whether the operation succeeds.

     If selection mode is None, this methods fails; and if selection mode is Single,
     the selected item would be changed to the specified item.
     */
    bool SelectItemAtIndex(std::size_t index);

    /**
     Scroll to the item at specified index.

     @param index
        The index of item scrolled to.

     @return 
        Return an value indicating that whether the operation succeeds.
     */
    bool ScrollToItemAtIndex(std::size_t index);

private:
    class ItemContainer : public Control {
    public:
        ItemContainer(const std::shared_ptr<ListBox>& list_box) : 
            list_box_(list_box),
            is_changing_selection_(false),
            is_mouse_down_item_originally_selected_(false) {
        
        }

        void Initialize() override;

        void RemoveItem(const std::shared_ptr<Item>& item);

    protected:
        void MouseDown(const Point& position, const MouseMessage& message) override;
        void MouseMove(const Point& position, const MouseMessage& message) override;
        void MouseUp(const Point& position, const MouseMessage& message) override;
        bool KeyDown(const KeyMessage& message) override;

    private:
        void LayoutItems(
            const std::shared_ptr<Control>&,
            const Rect& preivous_rect,
            const std::vector<std::shared_ptr<Control>>& children
        );

        bool SelectItemAtPositionByMouseEvent(const Point& position, bool is_mouse_moving);
        void SingleSelectItemByMouseEvent(const std::shared_ptr<ListBox>& list_box, const std::shared_ptr<Item>& item);
        void SimpleMultiSelectItemByMouseEvent(
            const std::shared_ptr<ListBox>& list_box,
            const std::shared_ptr<Item>& item, 
            bool is_mouse_moving
        );

        void ExtendedMultiSelectItemByMouseEvent(
            const std::shared_ptr<ListBox>& list_box,
            const std::shared_ptr<Item>& item,
            bool is_mouse_moving
        );
        void ExtendedMultiSelectItemWithMouseOnly(
            const std::shared_ptr<ListBox>& list_box, 
            const std::shared_ptr<Item>& item,
            bool is_mouse_moving
        );
        void ExtendedMultiSelectItemWithShiftKey(
            const std::shared_ptr<ListBox>& list_box,
            const std::shared_ptr<Item>& item,
            bool is_mouse_moving
        );
        void ExtendedMultiSelectItemWithControlKey(
            const std::shared_ptr<ListBox>& list_box,
            const std::shared_ptr<Item>& item,
            bool is_mouse_moving
        );
        void ChangeSelectionBetweenItems(
            const std::shared_ptr<ListBox>& list_box,
            const std::shared_ptr<Item>& begin_item,
            const std::shared_ptr<Item>& end_item,
            bool is_selecting,
            bool only_change_items_in_range
        );

        bool SingleSelectItemByKeyEvent(const std::shared_ptr<ListBox>& list_box, const KeyMessage& key_message);
        bool ExtendedMultiSelectItemByKeyEvent(const std::shared_ptr<ListBox>& list_box, const KeyMessage& key_message);

        bool ChangeIndexWithKeyMessage(
            const std::shared_ptr<ListBox>& list_box,
            std::size_t previous_index, 
            const KeyMessage& message,
            std::size_t& new_index
        ) const;

        void UpdateLastItem(const std::shared_ptr<Item>& item) {
            last_item_ = item;
            last_item_with_shift_key_ = item;
        }

    private:
        std::weak_ptr<ListBox> list_box_;
        std::shared_ptr<Item> last_item_;
        std::shared_ptr<Item> last_item_with_shift_key_;

        bool is_changing_selection_;
        std::shared_ptr<Item> mouse_down_item_;
        bool is_mouse_down_item_originally_selected_;
        std::set<std::shared_ptr<Item>> originally_selected_items_;
    };

private:
    bool CanMultiSelect() const {
        return GetSelectionMode() != SelectionMode::Single;
    }

    void UpdateScrollAreaSize();
    void RemoveItem(const std::shared_ptr<Item>& item);
    void SelectItem(const std::shared_ptr<Item>& item, bool unselect_others, bool scroll_to_it);
    void UnselectItem(const std::shared_ptr<Item>& item, bool scroll_to_it);
    std::size_t GetItemIndex(const std::shared_ptr<Item>& item) const;
    void ScrollToItem(const std::shared_ptr<Item>& item);
    void SelectionChange();

private:
    std::vector<std::shared_ptr<Item>> items_;
    std::shared_ptr<ItemContainer> item_container_;
};

}