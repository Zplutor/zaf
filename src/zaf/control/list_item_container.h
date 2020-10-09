#pragma once

#include <zaf/control/control.h>

namespace zaf {
namespace internal {
class ListControlImplementation;
class ListControlSelectStrategy;
}

/**
 Represents a container control that manages items in a list control.
 */
class ListItemContainer : public Control {
public:
    ListItemContainer();

protected:
    void Initialize() override;
    bool MouseDown(const Point& position, const MouseMessage& message) override;
    bool MouseMove(const Point& position, const MouseMessage& message) override;
    bool MouseUp(const Point& position, const MouseMessage& message) override;
    bool KeyDown(const KeyMessage& message) override;

private:
    friend class internal::ListControlImplementation;

    void SetSelectStrategy(const std::shared_ptr<internal::ListControlSelectStrategy>& select_strategy) {
        select_strategy_ = select_strategy;
    }

private:
    void LayoutItems(
        const Control& parent,
        const Rect& previous_rect,
        const std::vector<std::shared_ptr<Control>>& children);

private:
    std::shared_ptr<internal::ListControlSelectStrategy> select_strategy_;
};

}