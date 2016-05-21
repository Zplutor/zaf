#pragma once

#include <functional>
#include <zaf/control/scrollable_control.h>
#include <zaf/control/textual_control.h>

namespace zaf {

class ListBox : public ScrollableControl {
public:
    class Item : public TextualControl {
    
    };

    typedef std::function<const std::shared_ptr<Item>(const ListBox&)> ItemCreator;

public:
    ListBox();
    ~ListBox();

    void Initialize() override;

    float GetItemHeight() const;
    void SetItemHeight(float item_height);

    const ItemCreator GetItemCreator() const;
    void SetItemCreator(const ItemCreator& creator);

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

private:
    void ItemLayouter(
        const std::shared_ptr<Control>& scrolled_control,
        const Rect& preivous_rect,
        const std::vector<std::shared_ptr<Control>>& children
    );
    void UpdateScrollAreaSize();
    void RemoveItemFromScrolledControl(const std::shared_ptr<Item>& item);

private:
    std::vector<std::shared_ptr<Item>> items_;
};

}