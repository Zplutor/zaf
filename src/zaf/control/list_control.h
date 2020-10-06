#pragma once

#include <any>
#include <deque>
#include <zaf/base/define.h>
#include <zaf/base/event.h>
#include <zaf/control/list_item.h>
#include <zaf/control/scrollable_control.h>
#include <zaf/control/textual_control.h>
#include <zaf/creation.h>
#include <zaf/internal/no_circular_pointer.h>

namespace zaf {
namespace internal {
class ListControlExtendedMultipleSelectStrategy;
class ListControlItemHeightManager;
class ListControlItemSelectionManager;
class ListControlSelectStrategy;
class ListControlSimpleMultipleSelectStrategy;
class ListControlSingleSelectStrategy;
}

class ListItemContainer;
class ListControlDelegate;
class ListDataSource;

/**
 Represents a list control.

 This is the base class of all controls that display a list of items.

 A list control retrieves item information from the item source, such as the total
 count of items and the height of each item. You must create your own item source 
 which is derived from ListItemSource, and call SetItemSource method to set it into the 
 list control. 
 */
class ListControl : public ScrollableControl {
public:
    ZAF_DECLARE_REFLECTION_TYPE

public:
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

    std::shared_ptr<ListDataSource> GetDataSource() const {
        return data_source_.GetSharedPointer();
    }

    void SetDataSource(const std::shared_ptr<ListDataSource>& data_source);

    std::shared_ptr<ListControlDelegate> GetDelegate() const {
        return delegate_.GetSharedPointer();
    }

    void SetDelegate(const std::shared_ptr<ListControlDelegate>& delegate);

    /**
     Get item container.
     */
    const std::shared_ptr<ListItemContainer>& GetItemContainer() const {
        return item_container_;
    }

    /**
     Set item container.

     If nullptr is set, a default item container would be used.
     */
    void SetItemContainer(const std::shared_ptr<ListItemContainer>& item_container);

    /**
     Reload items.
     */
    void Reload();

    /**
     Get total count of items.
     */
    std::size_t GetItemCount() const;

    std::shared_ptr<Object> GetItemDataAtIndex(std::size_t index) const;

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
     Get data of the first selected item.

     @return
         Return nullptr if there is no selection.
     */
    std::shared_ptr<Object> GetFirstSelectedItemData() const;

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
    void Initialize() override;
    void Layout(const Rect& previous_rect) override;
    void VerticalScrollBarChange(const std::shared_ptr<ScrollBar>& previous_scroll_bar) override;

    /**
     This method is called when the data source is changed. Derived classes must call the same 
     method of base class.
     */
    virtual void DataSourceChange(const std::shared_ptr<ListDataSource>& previous_data_source) { }

    virtual void DelegateChange(const std::shared_ptr<ListControlDelegate>& previous_delegate) { }

    /**
     This method is called when the item container is changed. Derived classes must call the same 
     method of base class.
     */
    virtual void ItemContainerChange(const std::shared_ptr<ListItemContainer>& previous_item_container) { }

    void IsEnabledChange() override;

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
    void InitializeDataSource();
    void UninitializeDataSource();
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
    std::vector<std::shared_ptr<ListItem>> CreateItems(std::size_t index, std::size_t count);
    std::shared_ptr<ListItem> CreateItem(std::size_t index) const;

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

    std::shared_ptr<internal::ListControlSelectStrategy> CreateSelectStrategy();
    void ChangeSelection(std::size_t index, std::size_t count, bool is_add);

private:
    std::shared_ptr<ListItemContainer> item_container_;
    internal::NoCircularPointer<ListDataSource> data_source_;
    internal::NoCircularPointer<ListControlDelegate> delegate_;
    
    std::shared_ptr<internal::ListControlItemHeightManager> item_height_manager_;
    std::unique_ptr<internal::ListControlItemSelectionManager> item_selection_manager_;

    std::size_t first_visible_item_index_{};
    std::deque<std::shared_ptr<ListItem>> visible_items_;
};


/**
 Represents a container control that manages items in a list control.
 */
class ListItemContainer : public Control {
public:
    ListItemContainer();

protected:
    void Initialize() override;
    bool MouseDown(const Point& position, const MouseMessage& message) override;
    bool MouseMove(const Point& position, const MouseMessage& message) override;
    bool MouseUp(const Point& position, const MouseMessage& message) override;
    bool KeyDown(const KeyMessage& message) override;

private:
    friend class ListControl;

    void SetSelectStrategy(const std::shared_ptr<internal::ListControlSelectStrategy>& select_strategy) {
        select_strategy_ = select_strategy;
    }

private:
    void LayoutItems(
        const Control& parent,
        const Rect& previous_rect,
        const std::vector<std::shared_ptr<Control>>& children);

private:
    std::shared_ptr<internal::ListControlSelectStrategy> select_strategy_;
};

}