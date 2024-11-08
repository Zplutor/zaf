#include <zaf/control/property_grid/type_config.h>
#include <zaf/control/property_grid/text_editor.h>
#include <zaf/creation.h>

namespace zaf::property_grid {

std::shared_ptr<ValueEditor> TypeConfig::CreateValueView() {
    return Create<TextEditor>();
}

}
