#pragma once

#include <memory>
#include <zaf/base/non_copyable.h>
#include <zaf/control/list_item.h>
#include <zaf/creation.h>

namespace zaf {

class ListControlDelegate : NonCopyableNonMovable {
public:
    ListControlDelegate() = default;
    virtual ~ListControlDelegate() = default;

    /**
     Get a value indicating that whether each item has different height.
     */
    virtual bool HasVariableItemHeight() {
        return false;
    }

    /**
     Estimate the height of specified item.

     Note that if HasVariableItemHeight returns false, this method should always
     return the same height.
     */
    virtual float EstimateItemHeight(
        std::size_t item_index, 
        const std::shared_ptr<Object>& item_data) {

        return 0;
    }

    virtual float GetItemSpacing() {
        return 0;
    }

    virtual std::wstring GetItemText(
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) {

        return {};
    }

    virtual std::shared_ptr<ListItem> CreateItem(
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) {

        return Create<ListItem>();
    }

    virtual void LoadItem(const std::shared_ptr<ListItem>& item, std::size_t item_index) { }
};

}