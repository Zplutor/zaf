#pragma once

#include <functional>
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
            is_selected_ = is_selected;
            NeedRepaint();
        }

    private:
        bool is_selected_;
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

    bool AllowMultiSelect() const;
    void SetAllowMultiSelect(bool allow);

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

private:
    class ItemContainer : public Control {
    public:
        ItemContainer(const std::shared_ptr<ListBox>& list_box) : list_box_(list_box) { }

        void Initialize() override;

    protected:
        void MouseDown(const Point& position, const MouseMessage& message) override;
        void MouseUp(const Point& position, const MouseMessage& message) override;
        void MouseMove(const Point& position, const MouseMessage& message) override;

    private:
        void LayoutItems(
            const std::shared_ptr<Control>&,
            const Rect& preivous_rect,
            const std::vector<std::shared_ptr<Control>>& children
        );

        void SelectItemAtPosition(const Point& position);

    private:
        std::weak_ptr<ListBox> list_box_;
        std::shared_ptr<Item> selecting_item_;
    };

    void UpdateScrollAreaSize();
    void RemoveItemFromContainer(const std::shared_ptr<Item>& item);
    void SelectionChange();

private:
    std::vector<std::shared_ptr<Item>> items_;
    std::shared_ptr<ItemContainer> item_container_;
};

}