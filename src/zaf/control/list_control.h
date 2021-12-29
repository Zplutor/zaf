#pragma once

#include <deque>
#include <zaf/base/define.h>
#include <zaf/control/list_item.h>
#include <zaf/control/scrollable_control.h>
#include <zaf/control/selection_mode.h>
#include <zaf/control/textual_control.h>
#include <zaf/creation.h>
#include <zaf/internal/no_circular_pointer.h>
#include <zaf/rx/observable.h>

namespace zaf {
namespace internal {
class ListControlImplementation;
}

class ListItemContainer;
class ListControlDelegate;
class ListControlSelectionChangeInfo;
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
    ZAF_DECLARE_TYPE

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

    bool AutoAdjustScrollBarSmallChange() const;
    void SetAutoAdjustScrollBarSmallChange(bool value);

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
    Observable<ListControlSelectionChangeInfo> SelectionChangeEvent();

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
    std::vector<std::size_t> GetAllSelectedItemIndexes() const;

    /**
     Get the index of the first selected item.
     */
    std::optional<std::size_t> GetFirstSelectedItemIndex() const;

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
         Returns the index of item found, or std::nullopt if no item is found.
     */
    std::optional<std::size_t> FindItemIndexAtPosition(const Point& position) const;

protected:
    void Initialize() override;
    void Layout(const zaf::Rect& previous_rect) override;
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

    void OnIsEnabledChanged() override;

private:
    void SelectionChange();

private:
    std::shared_ptr<ListItemContainer> item_container_;
    internal::NoCircularPointer<ListDataSource> data_source_;
    internal::NoCircularPointer<ListControlDelegate> delegate_;
    std::shared_ptr<internal::ListControlImplementation> implementation_;
};


class ListControlSelectionChangeInfo {
public:
    ListControlSelectionChangeInfo(const std::shared_ptr<ListControl>& list_control) : 
        list_control_(list_control) {

    }

    const std::shared_ptr<ListControl>& ListControl() const {
        return list_control_;
    }

private:
    std::shared_ptr<zaf::ListControl> list_control_;
};

}