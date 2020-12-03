#pragma once

#include <zaf/control/tree_item.h>

namespace zaf::internal {

class InspectItem : public TreeItem {
protected:
    void Initialize() override;

    void MouseEnter(const std::shared_ptr<Control>& entered_control) override;
    void MouseLeave(const std::shared_ptr<Control>& leaved_control) override;
};

}