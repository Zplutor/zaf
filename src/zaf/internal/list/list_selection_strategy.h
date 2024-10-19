#pragma once

#include <memory>
#include <zaf/base/non_copyable.h>
#include <zaf/internal/list/list_item_height_manager.h>

namespace zaf {
class KeyMessage;
class MouseMessage;
class Point;
}

namespace zaf::internal {
    
class ListControlPartContext;

class ListSelectionStrategy : NonCopyableNonMovable {
public:
    explicit ListSelectionStrategy(const ListControlPartContext* context);
    virtual ~ListSelectionStrategy() = default;

    ListControlCore& GetListControl() const;
    ListItemHeightManager& GetItemHeightManager() const;

    virtual void BeginChangingSelectionByMouseDown(const Point& position_in_item_container) {

    }

    virtual void ChangeSelectionByMouseMove(const Point& position_in_item_container) {
    
    }

    virtual void EndChangingSelectionByMouseUp(const Point& position_in_item_container) {

    }

    virtual bool ChangeSelectionByKeyDown(const KeyMessage& message) {
        return false;
    }

protected:
    bool ChangeIndexByKeyDown(
        const KeyMessage& key_message, 
        const std::optional<std::size_t>& previous_index, 
        std::size_t& new_index);
    
private:
    const ListControlPartContext* part_context_{};
};


using ListNoSelectionStrategy = ListSelectionStrategy;

}