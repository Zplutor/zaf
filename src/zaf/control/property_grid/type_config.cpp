#include <zaf/control/property_grid/type_config.h>
#include <zaf/control/property_grid/text_value_view.h>
#include <zaf/creation.h>

namespace zaf::property_grid {

std::shared_ptr<ValueView> TypeConfig::CreateValueView() {
    return Create<TextValueView>();
}

}