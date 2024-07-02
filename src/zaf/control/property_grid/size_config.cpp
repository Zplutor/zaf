#include <zaf/control/property_grid/size_config.h>
#include <zaf/control/property_grid/editable_value_view.h>
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


std::shared_ptr<ValueView> SizeConfig::CreateValueView() {
    return Create<EditableValueView>();
}

}