#pragma once

#include <functional>
#include <zaf/base/define.h>
#include <zaf/base/event.h>
#include <zaf/control/scrollable_control.h>
#include <zaf/control/textual_control.h>

namespace zaf {

class ListBox : public ScrollableControl {
public:
    class Item : public TextualControl {
    public:
        Item() : is_selected_(false) { }

        void Initialize() override;

        bool IsSelected() const {
            return is_selected_;
        }

        void SetIsSelected(bool is_selected) {
            if (is_selected_ != is_selected) {
                is_selected_ = is_selected;
                NeedRepaint();
            }
        }

    private:
        bool is_selected_;
    };

    enum class SelectOption {
        SingleSelect,
        SimpleMultiSelect,
        ExtendedMultiSelect,
    };

    typedef std::function<const std::shared_ptr<Item>(const ListBox&)> ItemCreator;

    typedef Event<const std::shared_ptr<ListBox>&> SelectionChangeEvent;

public:
    ListBox();
    ~ListBox();

    void Initialize() override;

    float GetItemHeight() const;
    void SetItemHeight(float item_height);

    const ItemCreator GetItemCreator() const;
    void SetItemCreator(const ItemCreator& creator);

    SelectOption GetSelectOption() const;
    void SetSelectOption(SelectOption select_option);

    SelectionChangeEvent::Proxy GetSelectionChangeEvent();

    std::size_t AddItemWithText(const std::wstring& item_text) {
        return AddItemWithTextAtIndex(item_text, items_.size());
    }

    std::size_t AddItemWithTextAtIndex(const std::wstring& item_text, std::size_t index);

    std::size_t RemoveItemWithText(const std::wstring& item_text);
    bool RemoveItemAtIndex(std::size_t index);

    std::size_t GetItemCount() const {
        return items_.size();
    }

    const std::wstring GetItemTextAtIndex(std::size_t index);

    std::size_t GetFirstSelectedItemIndex() const;
    const std::wstring GetFirstSelectedItemText() const;

    const std::vector<std::size_t> GetSelectedItemIndexes() const;
    const std::vector<std::wstring> GetSelectedItemTexts() const;

    bool SelectItemAtIndex(std::size_t index);

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
        void KeyDown(const KeyMessage& message) override;

    private:
        void LayoutItems(
            const std::shared_ptr<Control>&,
            const Rect& preivous_rect,
            const std::vector<std::shared_ptr<Control>>& children
        );

        void SelectItemAtPositionByMouseEvent(const Point& position, bool is_mouse_moving);
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

        void SingleSelectItemByKeyEvent(const std::shared_ptr<ListBox>& list_box, const KeyMessage& key_message);
        void ExtendedMultiSelectItemByKeyEvent(const std::shared_ptr<ListBox>& list_box, const KeyMessage& key_message);

        std::size_t ChangeIndexWithKeyMessage(
            const std::shared_ptr<ListBox>& list_box,
            std::size_t previous_index, 
            const KeyMessage& message
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
        return GetSelectOption() != SelectOption::SingleSelect;
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