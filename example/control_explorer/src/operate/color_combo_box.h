#pragma once

#include <zaf/control/combo_box.h>

class ColorComboBox : public zaf::ComboBox {
public:
    void Initialize() override;

    zaf::Color GetColor() const;
    void SetColor(const zaf::Color& color);
};