#include <zaf/control/property_grid/size_config.h>
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

}