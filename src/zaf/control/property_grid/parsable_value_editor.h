#pragma once

#include <zaf/control/property_grid/value_editor.h>
#include <zaf/control/rich_edit.h>

namespace zaf::property_grid {

class ParsableValueEditor : public ValueEditor {
public:
    void SetAccessMethod(AccessMethod) override;
    void SetValue(const std::shared_ptr<Object>& value) override;

protected:
    void Initialize() override;
    void OnRichEditFocusLost(const FocusLostInfo& event_info);
    void ChangeValue();

private:
    std::shared_ptr<Object> value_;
    std::shared_ptr<RichEdit> rich_edit_;
};

}