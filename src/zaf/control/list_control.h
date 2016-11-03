#pragma once

#include <deque>
#include <zaf/base/define.h>
#include <zaf/base/event.h>
#include <zaf/control/scrollable_control.h>
#include <zaf/control/textual_control.h>

namespace zaf {

namespace internal {
class ListControlExtendedMultipleSelectStrategy;
class ListControlItemHeightManager;
class ListControlItemSelectionManager;
class ListControlSelectStrategy;
class ListControlSimpleMultipleSelectStrategy;
class ListControlSingleSelectStrategy;
}

class ListControl : public ScrollableControl {
public:
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

    class ItemSource : public std::enable_shared_from_this<ItemSource> {
    public:
        typedef Event<const std::shared_ptr<ItemSource>&, std::size_t, std::size_t> ItemAddEvent;
        typedef Event<const std::shared_ptr<ItemSource>&, std::size_t, std::size_t> ItemRemoveEvent;
        typedef Event<const std::shared_ptr<ItemSource>&, std::size_t, std::size_t> ItemUpdateEvent;

    public:
        ItemSource() {  }
        virtual ~ItemSource() { }

        virtual std::size_t GetItemCount() { 
            return 0;
        }

        virtual bool HasVariableItemHeight() {
            return false;
        }

        virtual float GetItemHeight(std::size_t index) {
            return 0;
        }

        virtual std::shared_ptr<Item> CreateItem(std::size_t index) {
            return nullptr;
        }

        ItemAddEvent::Proxy GetItemAddEvent() {
            return ItemAddEvent::Proxy(item_add_event_);
        }

        ItemRemoveEvent::Proxy GetItemRemoveEvent() {
            return ItemRemoveEvent::Proxy(item_remove_event_);
        }

        ItemRemoveEvent::Proxy GetItemUpdateEvent() {
            return ItemUpdateEvent::Proxy(item_update_event_);
        }

        ItemSource(const ItemSource&) = delete;
        ItemSource& operator=(const ItemSource&) = delete;

    protected:
        void NotifyItemAdd(std::size_t index, std::size_t count);
        void NotifyItemRemove(std::size_t index, std::size_t count);
        void NotifyItemUpdate(std::size_t index, std::size_t count);

    private:
        ItemAddEvent item_add_event_;
        ItemRemoveEvent item_remove_event_;
        ItemUpdateEvent item_update_event_;
    };

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

    const std::shared_ptr<ItemSource>& GetItemSource() const {
        return item_source_;
    }

    void SetItemSource(const std::shared_ptr<ItemSource>& item_source);
    void Reload();

    std::size_t GetItemCount() const;

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

    SelectionChangeEvent::Proxy GetSelectionChangeEvent();

    void SelectItemAtIndex(std::size_t index);
    void UnselectItemAtIndex(std::size_t index);

    void SelectAllItems();
    void UnselectAllItems();

    std::size_t GetSelectedItemCount() const;
    std::vector<std::size_t> GetSelectedItemIndexes() const;
    std::size_t GetFirstSelectedItemIndex() const;
    bool IsItemSelectedAtIndex(std::size_t index) const;

    void ScrollToItemAtIndex(std::size_t index);

protected:
    void Layout(const Rect& previous_rect) override;
    void VerticalScrollBarChange(const std::shared_ptr<ScrollBar>& previous_scroll_bar) override;

    virtual void ItemSourceChange(const std::shared_ptr<ItemSource>& previous_item_source) { }

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

    void ItemAdd(const std::shared_ptr<ItemSource>& item_source, std::size_t index, std::size_t count);
    void AddItemsBeforeVisibleItems(std::size_t index, std::size_t count, float position_difference);
    void AddItemsInMiddleOfVisibleItems(std::size_t index, std::size_t count, float position_difference);

    void ItemRemove(const std::shared_ptr<ItemSource>& item_source, std::size_t index, std::size_t count);
    void RemoveItemsBeforeVisibleItems(std::size_t index, std::size_t count, float position_difference);
    void RemoveItemsInMiddleOfVisibleItems(std::size_t index, std::size_t count, float position_difference);

    void ItemUpdate(const std::shared_ptr<ItemSource>& item_source, std::size_t index, std::size_t count);
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