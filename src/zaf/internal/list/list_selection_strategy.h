#pragma once

#include <memory>
#include <zaf/base/non_copyable.h>
#include <zaf/internal/list/list_control_parts_based.h>
#include <zaf/internal/list/list_item_height_manager.h>

namespace zaf {
class KeyMessage;
class Point;
}

namespace zaf::internal {
    
class ListControlPartsContext;

class ListSelectionStrategy : public ListControlPartsBased {
public:
    using ListControlPartsBased::ListControlPartsBased;
    virtual ~ListSelectionStrategy() = default;

    virtual void ChangeSelectionOnMouseDown(std::size_t item_index) {

    }

    virtual void ChangeSelectionOnMouseMove(std::size_t item_index) {

    }

    virtual void ChangeSelectionOnMouseUp(std::size_t item_index) {

    }

    virtual std::optional<std::size_t> ChangeSelectionOnKeyDown(const KeyMessage& message) {
        return std::nullopt;
    }

protected:
    bool ChangeIndexByKeyDown(
        const KeyMessage& key_message, 
        const std::optional<std::size_t>& previous_index, 
        std::size_t& new_index);
};


using ListNoSelectionStrategy = ListSelectionStrategy;

}