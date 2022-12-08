#pragma once

#include <zaf/control/property_grid/control_config.h>

namespace zaf::property_grid {

class TextualControlConfig : public ControlConfig {
public:
    void FilterProperties(PropertyTable& property_table) override;
};

}