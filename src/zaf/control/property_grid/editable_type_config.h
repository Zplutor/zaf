#pragma once

#include <zaf/control/property_grid/type_config.h>
#include <zaf/control/property_grid/editable_editor.h>
#include <zaf/creation.h>

namespace zaf::property_grid {

class EditableTypeConfig : public TypeConfig {
public:
    std::shared_ptr<ValueEditor> CreateValueView() override {
        return Create<EditableEditor>();
    }
};

}