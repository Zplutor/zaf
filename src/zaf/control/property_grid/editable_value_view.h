#pragma once

#include <zaf/control/property_grid/value_view.h>
#include <zaf/control/text_box.h>

namespace zaf::property_grid {

class EditableValueView : public ValueView {
public:
    void SetAccessMethod(AccessMethod) override;
    void SetValue(const std::shared_ptr<Object>& value) override;

    void RecoverFocus() override;

protected:
    void Initialize() override;
    void ChangeValue();

private:
    std::shared_ptr<Object> value_;
    std::shared_ptr<TextBox> text_box_;
};

}