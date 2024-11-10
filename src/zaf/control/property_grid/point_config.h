#pragma once

#include <zaf/control/property_grid/editable_editor.h>
#include <zaf/control/property_grid/type_config.h>
#include <zaf/creation.h>

namespace zaf::property_grid {

class PointConfig : public TypeConfig {
public:
    std::shared_ptr<ValueEditor> CreateValueEditor() override {
        return Create<EditableEditor>();
    }
};

}