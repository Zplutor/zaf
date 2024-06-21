#include <zaf/control/property_grid/rect_config.h>
#include <zaf/graphic/rect.h>

namespace zaf::property_grid {

void RectConfig::FilterProperties(PropertyTable& property_table) {

    auto list = property_table.GetList(Rect::StaticType());
    if (list) {
        list->SortByNames({
            L"Position",
            L"Size",
            L"Left",
            L"Top",
            L"Right",
            L"Bottom",
        });
    }
}

}