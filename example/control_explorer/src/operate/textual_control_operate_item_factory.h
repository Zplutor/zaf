#pragma once

#include "operate/control_operate_item_factory.h"
#include <zaf/control/textual_control.h>

class TextualControlOperateItemFactory : public ControlOperateItemFactory {
protected:
    void AddOperateItems(std::vector<std::shared_ptr<zaf::Control>>& items) override;

private:
    std::shared_ptr<zaf::TextualControl> GetTextualControl() const {
        return std::dynamic_pointer_cast<zaf::TextualControl>(GetTargetControl());
    }

    std::shared_ptr<zaf::Control> CreateTextAlignmentPanel();
    std::shared_ptr<zaf::Control> CreateParagraphAlignmentPanel();
    std::shared_ptr<zaf::Control> CreateWordWrappingPanel();
    std::shared_ptr<zaf::Control> CreateTextItem();
};