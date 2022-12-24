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
    void OnMouseDown(const MouseDownInfo& event_info) override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;
    void OnMouseUp(const MouseUpInfo& event_info) override;
    void OnKeyDown(const KeyDownInfo& event_info) override;
    void OnFocusChanged() override;

private:
    friend class internal::ListControlImplementation;

    void SetSelectStrategy(
        const std::shared_ptr<internal::ListControlSelectStrategy>& select_strategy) {
        select_strategy_ = select_strategy;
    }

private:
    void LayoutItems(
        const Control& parent,
        const zaf::Rect& previous_rect,
        const std::vector<std::shared_ptr<Control>>& children);

private:
    std::shared_ptr<internal::ListControlSelectStrategy> select_strategy_;
};

}