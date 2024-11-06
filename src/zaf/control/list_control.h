#pragma once

#include <deque>
#include <zaf/base/define.h>
#include <zaf/control/event/list_control_event_infos.h>
#include <zaf/control/list_item.h>
#include <zaf/control/scroll_box.h>
#include <zaf/control/selection_mode.h>
#include <zaf/control/textual_control.h>
#include <zaf/creation.h>
#include <zaf/rx/observable.h>

namespace zaf::internal {
class ListControlPartsContext;
}

namespace zaf {

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
class ListControl : public ScrollBox {
public:
    ZAF_OBJECT;

public:
    ListControl();
    ~ListControl();

    std::shared_ptr<ListDataSource> DataSource() const noexcept;
    void SetDataSource(std::shared_ptr<ListDataSource> data_source);

    std::shared_ptr<ListControlDelegate> Delegate() const noexcept;
    void SetDelegate(const std::weak_ptr<ListControlDelegate>& delegate);

    /**
     Get item container.
     */
    const std::shared_ptr<ListItemContainer>& ItemContainer() const noexcept;

    /**
     Set item container.
     */
    void SetItemContainer(std::shared_ptr<ListItemContainer> item_container);

    /**
     Reload items.
     */
    void Reload();

    /**
    Get total count of items.
    */
    std::size_t ItemCount() const;

    std::shared_ptr<Object> GetItemDataAtIndex(std::size_t index) const;

    /**
    Gets the visible list item at the specified index.

    @param index
        The index of the list item.

    @return
        The visible list item at the specified index, or null if the index is out of the visible 
        range.
    */
    std::shared_ptr<ListItem> GetVisibleItemAtIndex(std::size_t index) const noexcept;

    bool AutoAdjustScrollBarSmallChange() const;
    void SetAutoAdjustScrollBarSmallChange(bool value);

    /**
     Get selection mode.

     The default selection mode is Single.
     */
    SelectionMode SelectionMode() const;

    /**
     Set selection mode.

     All selected items would be unselected if selection mode is set
     to None; and only the first selected item would remain selected
     if selection mode is set to Single.
     */
    void SetSelectionMode(zaf::SelectionMode selection_mode);

    /**
     Get selection changed event.

     This event is raised when selection is changed.
     */
    Observable<ListControlSelectionChangedInfo> SelectionChangedEvent() const;

    /**
    Selects the item at the specified index.

    @param index
        The index of the item to select.

    @pre
        The index is less than the total count of items.
        
    @throw zaf::PreconditionError
    */
    void SelectItemAtIndex(std::size_t index);

    /**
    Unselects the item at the specified index.

    @param index
        The index of the item to unselect.

    @pre
        The index is less than the total count of items.

    @throw zaf::PreconditionError
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
    std::size_t SelectedItemCount() const;

    /**
     Get indexes of selected items.
     */
    std::vector<std::size_t> SelectedItemIndexes() const;

    /**
    Gets the index of the first selected item.

    @return
        The index of the first selected item, or null if there is no selection.
    */
    std::optional<std::size_t> FirstSelectedItemIndex() const noexcept;

    /**
     Get data of the first selected item.

     @return
         Return nullptr if there is no selection.
     */
    std::shared_ptr<Object> FirstSelectedItemData() const;

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
         Returns the index of item found, or std::nullopt if no item is found.
     */
    std::optional<std::size_t> FindItemIndexAtPosition(const Point& position) const;

    Observable<ListControlItemDoubleClickInfo> ItemDoubleClickEvent() const;

    Observable<ListControlContextMenuInfo> ContextMenuEvent() const;

protected:
    void Initialize() override;
    void Layout(const zaf::Rect& previous_rect) override;

    void OnMouseDown(const MouseDownInfo& event_info) override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;
    void OnMouseUp(const MouseUpInfo& event_info) override;
    void OnKeyDown(const KeyDownInfo& event_info) override;

    void OnFocusGained(const FocusGainedInfo& event_info) override;
    void OnFocusLost(const FocusLostInfo& event_info) override;

    void OnVerticalScrollBarChanged(
        const std::shared_ptr<ScrollBar>& previous_scroll_bar) override;

    /**
     This method is called when the data source is changed. Derived classes must call the same 
     method of base class.
     */
    virtual void OnDataSourceChanged(const std::shared_ptr<ListDataSource>& previous_data_source) { 

    }

    virtual void OnDelegateChanged(const std::shared_ptr<ListControlDelegate>& previous_delegate) { 
    
    }

    /**
     This method is called when the item container is changed. Derived classes must call the same 
     method of base class.
     */
    virtual void OnItemContainerChanged(
        const std::shared_ptr<ListItemContainer>& previous_item_container) { }

    virtual void OnSelectionChanged(const ListControlSelectionChangedInfo& event_info);

    virtual void OnContextMenu(const ListControlContextMenuInfo& event_info);

private:
    void OnCoreSelectionChanged();
    void OnCoreItemDoubleClick(std::size_t item_index);
    std::shared_ptr<PopupMenu> OnCoreContextMenu(
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data);

private:
    std::unique_ptr<internal::ListControlPartsContext> parts_;

    Event<ListControlSelectionChangedInfo> selection_changed_event_;
    Event<ListControlItemDoubleClickInfo> item_double_click_event_;
    Event<ListControlContextMenuInfo> context_menu_event_;
};

ZAF_OBJECT_BEGIN(ListControl);
ZAF_OBJECT_END;

}