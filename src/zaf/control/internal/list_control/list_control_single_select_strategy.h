#pragma once

#include <zaf/control/internal/list_control/list_control_select_strategy.h>

namespace zaf {
namespace internal {

class ListControlSingleSelectStrategy : public ListControlSelectStrategy {
public:
    void BeginChangingSelectionByMouseDown(const Point& position, const MouseMessage& message) override;
    void ChangeSelectionByMouseMove(const Point& position, const MouseMessage& message) override;
    void EndChangingSelectionByMouseUp(const Point& position, const MouseMessage& message) override;
    bool ChangeSelectionByKeyDown(const KeyMessage& message) override;

private:
    void SelectItemWithMouseEvent(const Point& position);
};

}
}