#include <zaf/control/property_grid/frame_config.h>
#include <zaf/graphic/frame.h>

namespace zaf::property_grid {

void FrameConfig::FilterProperties(PropertyTable& property_table) {

    auto list = property_table.GetList(Frame::Type);
    if (list) {
        list->SortByNames({
            L"Left",
            L"Top",
            L"Right",
            L"Bottom",
        });
    }
}

}