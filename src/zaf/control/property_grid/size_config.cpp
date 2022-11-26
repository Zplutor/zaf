#include <zaf/control/property_grid/size_config.h>
#include <zaf/control/property_grid/editable_value_view.h>
#include <zaf/creation.h>
#include <zaf/graphic/size.h>

namespace zaf::property_grid {

void SizeConfig::FilterProperties(PropertyTable& property_table) {

    auto list = property_table.GetList(Size::Type);
    if (list) {
        list->SortByNames({
            L"Width",
            L"Height",
        });
    }
}


std::shared_ptr<ValueView> SizeConfig::CreateValueView() {
    return Create<EditableValueView>();
}

}