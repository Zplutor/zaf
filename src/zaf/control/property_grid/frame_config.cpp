#include <zaf/control/property_grid/frame_config.h>
#include <zaf/control/property_grid/parsable_value_editor.h>
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


std::shared_ptr<ValueEditor> FrameConfig::CreateValueEditor() {
    return Create<ParsableValueEditor>();
}

}