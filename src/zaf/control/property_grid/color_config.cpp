#include <zaf/control/property_grid/color_config.h>
#include <zaf/control/property_grid/color_value_view.h>
#include <zaf/creation.h>

namespace zaf::property_grid {

void ColorConfig::FilterProperties(PropertyTable& property_table) {
    property_table.Clear();
}


std::shared_ptr<ValueView> ColorConfig::CreateValueView() {
    return Create<ColorValueView>();
}

}