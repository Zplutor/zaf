#include <zaf/control/property_grid/rect_config.h>
#include <zaf/graphic/rect.h>

namespace zaf::property_grid {

void RectConfig::FilterProperties(PropertyTable& property_table) {

    auto type = Rect::Type::Instance();

    auto list = property_table.GetList(type);
    if (list) {
        list->Sort({
            type->PositionProperty,
            type->SizeProperty,
            type->LeftProperty,
            type->TopProperty,
            type->RightProperty,
            type->BottomProperty,
        });
    }
}

}