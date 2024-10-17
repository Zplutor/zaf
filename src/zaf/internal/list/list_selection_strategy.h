#pragma once

#include <memory>
#include <zaf/base/non_copyable.h>
#include <zaf/internal/list/list_control_core.h>

namespace zaf {

class KeyMessage;
class MouseMessage;
class Point;

namespace internal {
    
class ListControlItemHeightManager;

class ListSelectionStrategy : NonCopyableNonMovable {
public:
    ListSelectionStrategy() = default;
    virtual ~ListSelectionStrategy() = default;

    std::shared_ptr<ListControlCore> GetListControl() const {
        return list_control_.lock();
    }

    void SetListControl(const std::weak_ptr<ListControlCore> list_control) {
        list_control_ = list_control;
    }

    const std::shared_ptr<ListControlItemHeightManager>& GetItemHeightManager() const {
        return item_height_manager_;
    }

    void SetItemHeightManager(std::shared_ptr<ListControlItemHeightManager> item_height_manager) {
        item_height_manager_ = std::move(item_height_manager);
    }

    virtual void BeginChangingSelectionByMouseDown(
        const Point& position, 
        const MouseMessage& message) {

    }

    virtual void ChangeSelectionByMouseMove(
        const Point& position,
        const MouseMessage& message) {
    
    }

    virtual void EndChangingSelectionByMouseUp(
        const Point& position, 
        const MouseMessage& message) {

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
    std::weak_ptr<ListControlCore> list_control_;
    std::shared_ptr<ListControlItemHeightManager> item_height_manager_;
};


using ListNoSelectionStrategy = ListSelectionStrategy;

}
}