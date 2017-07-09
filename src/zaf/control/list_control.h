#pragma once

#include <deque>
#include <zaf/base/define.h>
#include <zaf/base/event.h>
#include <zaf/control/scrollable_control.h>
#include <zaf/control/textual_control.h>
#include <zaf/creation.h>

namespace zaf {

namespace internal {
class ListControlExtendedMultipleSelectStrategy;
class ListControlItemHeightManager;
class ListControlItemSelectionManager;
class ListControlSelectStrategy;
class ListControlSimpleMultipleSelectStrategy;
class ListControlSingleSelectStrategy;
}

/**
 Represents a list control.

 This is the base class of all controls that display a list of items.

 A list control retrieves item information from the item source, such as the total
 count of items and the height of each item. You must create your own item source 
 which is derived from ItemSource, and call SetItemSource method to set it into the 
 list control. 
 */
class ListControl : public ScrollableControl {
public:
    ZAF_DECLARE_TYPE_NAME();

public:
    /**
     Represents an item in list control.

     The default item displays text only. You can dervied from it to implement a more 
     complex item.
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

        /**
         Set a value indicating that whether the item is selected.
         */
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
     Represents an item source that is used in list control.

     The default item source doesn't manage any item, and is readonly. You must 
     derived from this class to implement your own item source.  
     */
    class ItemSource {
    public:
        /**
         Type of item add event.
         */
        typedef Event<ItemSource&, std::size_t, std::size_t> ItemAddEvent;

        /**
         Type of item remove event.
         */
        typedef Event<ItemSource&, std::size_t, std::size_t> ItemRemoveEvent;

        /**
         Type of item update event.
         */
        typedef Event<ItemSource&, std::size_t, std::size_t> ItemUpdateEvent;

    public:
        ItemSource() { }
        virtual ~ItemSource() { }

        /**
         Get total count of items.
         */
        virtual std::size_t GetItemCount() { 
            return 0;
        }

        /**
         Get a value indicating that whether each item has different height.
         */
        virtual bool HasVariableItemHeight() {
            return false;
        }

        /**
         Get the height of item at specified index.

         Note that if HasVariableItemHeight returns false, this method should always 
         return the same height regardless of the index.
         */
        virtual float GetItemHeight(std::size_t index) {
            return 0;
        }

        /**
         Create an item for specified index.

         This method should not return nullptr. However, a default item would be created
         if nullptr is returned indeed.

         You should create the item instance in this method, and set related data to the 
         item in LoadItem method.
         */
        virtual std::shared_ptr<Item> CreateItem(std::size_t index) {
            return Create<Item>();
        }

        /**
         Load data into specified item.
         */
        virtual void LoadItem(std::size_t index, const std::shared_ptr<Item>& item) { }

        /**
         Get item add event.
         */
        ItemAddEvent::Proxy GetItemAddEvent() {
            return ItemAddEvent::Proxy(item_add_event_);
        }

        /**
         Get item remove event.
         */
        ItemRemoveEvent::Proxy GetItemRemoveEvent() {
            return ItemRemoveEvent::Proxy(item_remove_event_);
        }

        /**
         Get item update event.
         */
        ItemRemoveEvent::Proxy GetItemUpdateEvent() {
            return ItemUpdateEvent::Proxy(item_update_event_);
        }

        ItemSource(const ItemSource&) = delete;
        ItemSource& operator=(const ItemSource&) = delete;

    protected:
        /**
         Raise an item add event.

         @param index
             The index where items are added.

         @param count
             The count of items added.

         You must call this method to update the list control after adding 
         items to item source.
         */
        void NotifyItemAdd(std::size_t index, std::size_t count);

        /**
         Raise an item remove event.

         @param index
             The index of the first item removed.

         @param count
             The count of items removed.

         You must call this method to update the list control after removing
         items from item source.
         */
        void NotifyItemRemove(std::size_t index, std::size_t count);

        /**
         Raise an item update event.

         @param index
             The index of the first item updated.

         @param count
             The count of items updated.

         You must call this method to update the list control after updating
         items in item source.
         */
        void NotifyItemUpdate(std::size_t index, std::size_t count);

    private:
        ItemAddEvent item_add_event_;
        ItemRemoveEvent item_remove_event_;
        ItemUpdateEvent item_update_event_;
    };

    /**
     Represents a container control that manages items in a list control.
     */
    class ItemContainer : public Control {
    public:
        ItemContainer();

        void Initialize() override;

    protected:
        void MouseDown(const Point& position, const MouseMessage& message) override;
        void MouseMove(const Point& position, const MouseMessage& message) override;
        void MouseUp(const Point& position, const MouseMessage& message) override;
        bool KeyDown(const KeyMessage& message) override;

    private:
        friend class ListControl;

        void SetSelectStrategy(const std::shared_ptr<internal::ListControlSelectStrategy>& select_strategy) {
            select_strategy_ = select_strategy;
        }

    private:
        void LayoutItems(
            const std::shared_ptr<Control>& parent,
            const Rect& previous_rect,
            const std::vector<std::shared_ptr<Control>>& children);

    private:
        std::shared_ptr<internal::ListControlSelectStrategy> select_strategy_;
    };

    /**
     Defines the selection mode of a list control.
     */
    enum class SelectionMode {

        /**
         The list control items can not be selected.
         */
        None,

        /**
         Only one single list control item can be selected.
         */
        Single,

        /**
         Multiple list control items can be selected with mouse clicking.
         */
        SimpleMultiple,

        /**
         Multiple list control items can be selected with the help of SHIFT and CTRL keys.
         */
        ExtendedMultiple,
    };

    /**
     The type of selection change event.
     */
    typedef Event<const std::shared_ptr<ListControl>&> SelectionChangeEvent;

public:
    ListControl();
    ~ListControl();

    void Initialize() override;

    /**
     Get item source.
     */
    const std::shared_ptr<ItemSource>& GetItemSource() const {
        return item_source_;
    }

    /**
     Set item source.

     If nullptr is set, a default item source would be used.
     */
    void SetItemSource(const std::shared_ptr<ItemSource>& item_source);

    /**
     Get item container.
     */
    const std::shared_ptr<ItemContainer> GetItemContainer() const {
        return item_container_;
    }

    /**
     Set item container.

     If nullptr is set, a default item container would be used.
     */
    void SetItemContainer(const std::shared_ptr<ItemContainer>& item_container);

    /**
     Reload items.
     */
    void Reload();

    /**
     Get total count of items.
     */
    std::size_t GetItemCount() const;

    /**
     Get item at specified index.

     @param index
        The index of item.

     @return 
        Returns item object, or nullptr if index is invalid.
     */
    const std::shared_ptr<Item> GetItemAtIndex(std::size_t index) const;

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

     This event is raised when selection is changed.
     */
    SelectionChangeEvent::Proxy GetSelectionChangeEvent();

    /**
     Select the item at specifed index.
     */
    void SelectItemAtIndex(std::size_t index);

    /**
     Unselect the item at specified index.
     */
    void UnselectItemAtIndex(std::size_t index);

    /**
     Select all items.
     */
    void SelectAllItems();

    /**
     Unselect all items.
     */
    void UnselectAllItems();

    /**
     Get total count of selected items.
     */
    std::size_t GetSelectedItemCount() const;

    /**
     Get indexes of selected items.
     */
    std::vector<std::size_t> GetSelectedItemIndexes() const;

    /**
     Get the index of the first selected item.
     */
    std::size_t GetFirstSelectedItemIndex() const;

    /**
     Determinate whether the item at specified index is selected.
     */
    bool IsItemSelectedAtIndex(std::size_t index) const;

    /**
     Scroll to the item at specified index.
     */
    void ScrollToItemAtIndex(std::size_t index);

    /**
     Find the index of item at specified position.

     @param position
         The position to be determinated, in current control's coordinate system.

     @return
         Returns the index of item found, or InvalidIndex if no item is found.
     */
    std::size_t FindItemIndexAtPosition(const Point& position) const;

protected:
    void Layout(const Rect& previous_rect) override;
    void VerticalScrollBarChange(const std::shared_ptr<ScrollBar>& previous_scroll_bar) override;

    /**
     This method is called when the item source is changed. Derived classes must call the same 
     method of base class.
     */
    virtual void ItemSourceChange(const std::shared_ptr<ItemSource>& previous_item_source) { }

    /**
     This method is called when the item container is changed. Derived classes must call the same 
     method of base class.
     */
    virtual void ItemContainerChange(const std::shared_ptr<ItemContainer>& previous_item_container) { }

    void IsEnabledChange() override;

    void DeserializeProperty(const std::wstring& name, const DataNode& data_node) override;

private:
    friend class internal::ListControlExtendedMultipleSelectStrategy;
    friend class internal::ListControlSingleSelectStrategy;
    friend class internal::ListControlSimpleMultipleSelectStrategy;

    void ReplaceSelection(std::size_t index, std::size_t count);
    void RevertSelection(std::size_t index);
    void AddSelection(std::size_t index, std::size_t count);
    void RemoveSelection(std::size_t index, std::size_t count);
    void NotifySelectionChange();

private:
    void InitializeScrollBar();
    void InitializeItemSource();
    void UninitializeItemSource();
    void UpdateContentHeight();
    void UpdateVisibleItems();
    void GetVisibleItemsRange(std::size_t& index, std::size_t& count) const;
    void AdjustVisibleItems(
        std::size_t new_index, 
        std::size_t new_count, 
        bool remove_head, 
        std::size_t head_change_count, 
        bool remove_tail, 
        std::size_t tail_change_count);
    void RemoveHeadVisibleItems(std::size_t count);
    void RemoveTailVisibleItems(std::size_t count);
    const std::vector<std::shared_ptr<Item>> CreateItems(std::size_t index, std::size_t count);
    const std::shared_ptr<Item> CreateItem(std::size_t index) const;

    void ItemAdd(std::size_t index, std::size_t count);
    void AddItemsBeforeVisibleItems(std::size_t index, std::size_t count, float position_difference);
    void AddItemsInMiddleOfVisibleItems(std::size_t index, std::size_t count, float position_difference);

    void ItemRemove(std::size_t index, std::size_t count);
    void RemoveItemsBeforeVisibleItems(std::size_t index, std::size_t count, float position_difference);
    void RemoveItemsInMiddleOfVisibleItems(std::size_t index, std::size_t count, float position_difference);

    void ItemUpdate(std::size_t index, std::size_t count);
    void AdjustVisibleItemPositionsByUpdatingItems(std::size_t index, std::size_t count, float position_difference);
    void UpdateVisibleItemsByUpdatingItems(std::size_t index, std::size_t count);

    float AdjustContentHeight();
    void AdjustVisibleItemPositions(std::size_t begin_adjust_index, float difference);

    const std::shared_ptr<internal::ListControlSelectStrategy> CreateSelectStrategy();
    void ChangeSelection(std::size_t index, std::size_t count, bool is_add);

private:
    std::shared_ptr<ItemContainer> item_container_;
    std::shared_ptr<ItemSource> item_source_;
    
    std::shared_ptr<internal::ListControlItemHeightManager> item_height_manager_;
    std::unique_ptr<internal::ListControlItemSelectionManager> item_selection_manager_;

    std::size_t first_visible_item_index_;
    std::deque<std::shared_ptr<Item>> visible_items_;
};

}