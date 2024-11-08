#include <zaf/control/property_grid/color_config.h>
#include <zaf/control/property_grid/color_editor.h>
#include <zaf/creation.h>

namespace zaf::property_grid {

void ColorConfig::FilterProperties(PropertyTable& property_table) {
    property_table.Clear();
}


std::shared_ptr<ValueEditor> ColorConfig::CreateValueView() {
    return Create<ColorEditor>();
}

}