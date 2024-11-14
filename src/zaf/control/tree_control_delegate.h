#pragma once

#include <zaf/control/tree_item.h>
#include <zaf/creation.h>

namespace zaf {

class TreeControlDelegate {
public:
    virtual bool HasVariableItemHeight() {
        return false;
    }

    virtual float EstimateItemHeight(
        const std::shared_ptr<Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) {

        return 0;
    }

    virtual float GetItemSpacing() {
        return 0;
    }

    virtual std::shared_ptr<TreeItem> CreateItem(
        const std::shared_ptr<Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) {

        return Create<TreeItem>();
    }

    virtual bool CanExpandItem(
        const std::shared_ptr<Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) {

        return true;
    }

    virtual bool CanCollapseItem(
        const std::shared_ptr<Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) {

        return true;
    }
};

}