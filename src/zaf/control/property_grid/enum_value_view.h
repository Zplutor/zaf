#pragma once

#include <zaf/control/combo_box.h>
#include <zaf/control/property_grid/value_view.h>

namespace zaf::property_grid {

class EnumValueView : public ValueView {
public:
    void SetAccessMethod(AccessMethod) override;
    void SetValue(const std::shared_ptr<Object>& object) override;

protected:
    void Initialize() override;

private:
    void InitializeComboBoxValues(
        const std::vector<std::shared_ptr<Object>>& values, 
        const std::shared_ptr<Object>& selected_object);

private:
    std::shared_ptr<ComboBox> combo_box_;
};

}