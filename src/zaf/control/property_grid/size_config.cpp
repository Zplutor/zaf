#include <zaf/control/property_grid/size_config.h>
#include <zaf/control/property_grid/editable_editor.h>
#include <zaf/creation.h>
#include <zaf/graphic/size.h>

namespace zaf::property_grid {

void SizeConfig::FilterProperties(PropertyTable& property_table) {

    auto type = Size::Type::Instance();

    auto list = property_table.GetList(type);
    if (list) {
        list->Sort({
            type->WidthProperty,
            type->HeightProperty,
        });
    }
}


std::shared_ptr<ValueEditor> SizeConfig::CreateValueEditor() {
    return Create<EditableEditor>();
}

}