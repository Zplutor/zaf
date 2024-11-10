#include <zaf/control/property_grid/type_config.h>
#include <zaf/control/property_grid/view_only_editor.h>
#include <zaf/creation.h>

namespace zaf::property_grid {

std::shared_ptr<ValueEditor> TypeConfig::CreateValueEditor() {
    return Create<ViewOnlyEditor>();
}

}
