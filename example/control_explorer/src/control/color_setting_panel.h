#pragma once

#include <zaf/control/control.h>
#include <zaf/control/combo_box.h>

class ColorSettingPanel : public zaf::Control {
public:
    void Initialize() override;

    const zaf::Color GetColor() const;
    void SetColor(const zaf::Color& color);

private:
    void InitializeColorComboBox();

private:
    std::shared_ptr<zaf::ComboBox> color_combo_box_;
};