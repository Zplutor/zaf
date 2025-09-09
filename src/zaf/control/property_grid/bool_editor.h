#pragma once

#include <zaf/control/check_box.h>
#include <zaf/control/property_grid/value_editor.h>

namespace zaf::property_grid {

class BoolEditor : public ValueEditor {
public:
    void SetAccessMethod(AccessMethod) override;
    void SetValue(const std::shared_ptr<dynamic::Object>& object) override;

protected:
    void Initialize() override;

private:
    std::shared_ptr<CheckBox> check_box_;
};

}