#pragma once

#include <zaf/control/check_box.h>
#include <zaf/control/property_grid/value_view.h>

namespace zaf::property_grid {

class BoolValueView : public ValueView {
public:
    void SetAccessMethod(AccessMethod) override;
    void SetValue(const std::shared_ptr<Object>& object) override;
    void RecoverFocus() override;

protected:
    void Initialize() override;

private:
    std::shared_ptr<CheckBox> check_box_;
};

}