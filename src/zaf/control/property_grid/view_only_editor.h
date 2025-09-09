#pragma once

#include <zaf/control/label.h>
#include <zaf/control/property_grid/value_editor.h>

namespace zaf::property_grid {

class ViewOnlyEditor : public ValueEditor {
public:
    void SetAccessMethod(AccessMethod) override;
    void SetValue(const std::shared_ptr<dynamic::Object>& value) override;

protected:
    void Initialize() override;

private:
    std::shared_ptr<Label> label_;
};

}