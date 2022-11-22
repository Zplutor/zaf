#pragma once

#include <zaf/control/property_grid/type_config.h>
#include <zaf/control/property_grid/editable_value_view.h>
#include <zaf/creation.h>

namespace zaf::property_grid {

class EditableTypeConfig : public TypeConfig {
public:
    std::shared_ptr<ValueView> CreateValueView() override {
        return Create<EditableValueView>();
    }
};

}