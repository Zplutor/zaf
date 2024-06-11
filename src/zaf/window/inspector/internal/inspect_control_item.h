#pragma once

#include <zaf/window/inspector/internal/inspect_item.h>

namespace zaf::internal {

class InspectControlItem : public InspectItem {
public:
    InspectControlItem(const std::shared_ptr<Control>& control);

protected:
    void UpdateVisualState() override;

private:
    std::shared_ptr<Control> control_;
};

}