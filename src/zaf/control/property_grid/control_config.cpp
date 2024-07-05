#include <zaf/control/property_grid/control_config.h>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/erase.h>
#include <zaf/control/control.h>
#include <zaf/control/control_object.h>

namespace zaf::property_grid {

void ControlConfig::FilterProperties(PropertyTable& property_table) {

    auto type = Control::Type::Instance();

    auto list = property_table.GetList(type);
    if (!list) {
        return;
    }

    list->Erase({
        type->ParentProperty,
        type->WindowProperty,
        type->XProperty,
        type->YProperty,
        type->PositionProperty,
        type->PositionInWindowProperty,
        type->SizeProperty,
        type->WidthProperty,
        type->HeightProperty,
        type->ContentSizeProperty,
    });

    list->Sort({
        type->NameProperty,
        type->RectInWindowProperty,
        type->RectProperty,
        type->BorderProperty,
        type->PaddingProperty,
        type->MarginProperty,
        type->ContentRectProperty,
        type->MinWidthProperty,
        type->MaxWidthProperty,
        type->MinHeightProperty,
        type->MaxHeightProperty,
        type->AutoWidthProperty,
        type->AutoHeightProperty,
    });
}

}