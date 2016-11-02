#pragma once

#include <memory>

namespace zaf {

class KeyMessage;
class ListControl;
class MouseMessage;
class Point;

namespace internal {
    
class ListControlItemHeightManager;

class ListControlSelectStrategy {
public:
    ListControlSelectStrategy() { }
    virtual ~ListControlSelectStrategy() { }

    const std::shared_ptr<ListControl> GetListControl() const {
        return list_control_.lock();
    }

    void SetListControl(const std::shared_ptr<ListControl>& list_control) {
        list_control_ = list_control;
    }

    const std::shared_ptr<ListControlItemHeightManager>& GetItemHeightManager() const {
        return item_height_manager_;
    }

    void SetItemHeightManager(const std::shared_ptr<ListControlItemHeightManager>& item_height_manager) {
        item_height_manager_ = item_height_manager;
    }

    virtual void BeginChangingSelectionByMouseDown(const Point& position, const MouseMessage& message) { }
    virtual void ChangeSelectionByMouseMove(const Point& position, const MouseMessage& message) { }
    virtual void EndChangingSelectionByMouseUp(const Point& position, const MouseMessage& message) { }
    virtual bool ChangeSelectionByKeyDown(const KeyMessage& message) { return false; }

    ListControlSelectStrategy(const ListControlSelectStrategy&) = delete;
    ListControlSelectStrategy& operator=(const ListControlSelectStrategy&) = delete;

protected:
    bool ChangeIndexByKeyDown(const KeyMessage& key_message, std::size_t previous_index, std::size_t& new_index);
    
private:
    std::weak_ptr<ListControl> list_control_;
    std::shared_ptr<ListControlItemHeightManager> item_height_manager_;
};

typedef ListControlSelectStrategy ListControlNoSelectStrategy;

}
}