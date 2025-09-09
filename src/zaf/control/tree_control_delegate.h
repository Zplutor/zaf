#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/tree_item.h>
#include <zaf/creation.h>

namespace zaf {

class TreeControlDelegate : NonCopyableNonMovable {
public:
    static const std::shared_ptr<TreeControlDelegate>& Default();

public:
    TreeControlDelegate() = default;
    virtual ~TreeControlDelegate() = default;

    virtual bool HasVariableItemHeight() {
        return false;
    }

    virtual float EstimateItemHeight(
        const std::shared_ptr<dynamic::Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<dynamic::Object>& item_data) {

        return 0;
    }

    virtual float GetItemSpacing() {
        return 0;
    }

    virtual std::shared_ptr<TreeItem> CreateItem(
        const std::shared_ptr<dynamic::Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<dynamic::Object>& item_data) {

        return Create<TreeItem>();
    }

    virtual bool CanExpandItem(
        const std::shared_ptr<dynamic::Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<dynamic::Object>& item_data) {

        return true;
    }

    virtual bool CanCollapseItem(
        const std::shared_ptr<dynamic::Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<dynamic::Object>& item_data) {

        return true;
    }
};

}