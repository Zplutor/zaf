#pragma once

#include <zaf/control/text_box.h>
#include "manager/textual_control_explore_manager.h"

class TextBoxExploreManager : public TextualControlExploreManager {
public:
    std::shared_ptr<zaf::Control> CreateExploredControl() override;
    void CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items) override;

private:
    std::shared_ptr<zaf::TextBox> GetTextBox() {
        return std::dynamic_pointer_cast<zaf::TextBox>(GetTargetControl());
    }

    std::shared_ptr<PropertyItem> CreateAllowBeepPropertyItem();
    std::shared_ptr<PropertyItem> CreateReadOnlyPropertyItem();
    std::shared_ptr<PropertyItem> CreateIsMultilinePropertyItem();
    std::shared_ptr<PropertyItem> CreateMaximumLengthPropertyItem();
    std::shared_ptr<PropertyItem> CreateUsePasswordCharacterPropertyItem();
    std::shared_ptr<PropertyItem> CreatePasswordCharacterPropertyItem();
};