#pragma once

#include <zaf/control/control.h>

namespace zaf {
namespace internal {
class ListControlCore;
class ListSelectionStrategy;
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
    void OnFocusGained(const FocusGainedInfo& event_info) override;
    void OnFocusLost(const FocusLostInfo& event_info) override;

private:
    friend class internal::ListControlCore;

    void SetSelectStrategy(
        const std::shared_ptr<internal::ListSelectionStrategy>& select_strategy) {
        select_strategy_ = select_strategy;
    }

private:
    void LayoutItems(
        const Control& parent,
        const zaf::Rect& previous_rect,
        const std::vector<std::shared_ptr<Control>>& children);

    void RepaintSelectedItemsOnFocusChanged(
        const std::shared_ptr<Control>& focus_changed_control,
        const std::shared_ptr<Control>& focus_changing_control);

    bool AreControlsSelfOrChildren(
        const std::shared_ptr<Control>& control1,
        const std::shared_ptr<Control>& control2) const;

    void RepaintSelectedItems();

private:
    std::shared_ptr<internal::ListSelectionStrategy> select_strategy_;
};

}