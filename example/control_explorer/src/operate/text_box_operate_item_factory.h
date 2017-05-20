#pragma once

#include <zaf/control/text_box.h>
#include "textual_control_operate_item_factory.h"

class TextBoxOperateItemFactory : public TextualControlOperateItemFactory {
public:
    void AddOperateItems(std::vector<std::shared_ptr<zaf::Control>>& items) override;

private:
    std::shared_ptr<zaf::TextBox> GetTextBox() const {
        return std::dynamic_pointer_cast<zaf::TextBox>(GetTargetControl());
    }

    std::shared_ptr<zaf::Control> CreateAllowBeepOperateItem();
    std::shared_ptr<zaf::Control> CreateReadOnlyOperateItem();
    std::shared_ptr<zaf::Control> CreateIsMultilineOperateItem();
    std::shared_ptr<zaf::Control> CreateMaximumLengthOperateItem();
    std::shared_ptr<zaf::Control> CreateUsePasswordCharacterOperateItem();
    std::shared_ptr<zaf::Control> CreatePasswordCharacterOperateItem();
};