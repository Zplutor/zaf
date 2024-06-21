#include <zaf/control/property_grid/frame_config.h>
#include <zaf/control/property_grid/editable_value_view.h>
#include <zaf/creation.h>
#include <zaf/graphic/frame.h>

namespace zaf::property_grid {

void FrameConfig::FilterProperties(PropertyTable& property_table) {

    auto list = property_table.GetList(Frame::StaticType());
    if (list) {
        list->SortByNames({
            L"Left",
            L"Top",
            L"Right",
            L"Bottom",
            L"Width",
            L"Height",
        });
    }
}


std::shared_ptr<ValueView> FrameConfig::CreateValueView() {
    return Create<EditableValueView>();
}

}