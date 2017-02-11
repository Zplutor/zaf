#pragma once

#include <zaf/control/combo_box.h>
#include <zaf/control/control.h>
#include <zaf/graphic/font/font.h>

class FontSettingPanel : public zaf::Control {
public:
    void Initialize() override;

    zaf::Font GetFont() const;
    void SetFont(const zaf::Font& font);

private:
    void InitializeFamilyNamePanel();
    void InitializeSizePanel();
    void InitializeWeightPanel();

private:
    std::shared_ptr<zaf::ComboBox> family_name_combo_box_;
    std::shared_ptr<zaf::TextBox> size_text_box_;
    std::shared_ptr<zaf::TextBox> weight_text_box_;
};