#include <zaf/control/property_grid/frame_config.h>
#include <zaf/control/property_grid/editable_value_view.h>
#include <zaf/creation.h>
#include <zaf/graphic/frame.h>

namespace zaf::property_grid {

void FrameConfig::FilterProperties(PropertyTable& property_table) {

    auto type = Frame::Type::Instance();

    auto list = property_table.GetList(type);
    if (list) {
        list->Sort({
            type->LeftProperty,
            type->TopProperty,
            type->RightProperty,
            type->BottomProperty,
            type->WidthProperty,
            type->HeightProperty,
        });
    }
}


std::shared_ptr<ValueView> FrameConfig::CreateValueView() {
    return Create<EditableValueView>();
}

}