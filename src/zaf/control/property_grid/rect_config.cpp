#include <zaf/control/property_grid/rect_config.h>

namespace zaf::property_grid {

void RectConfig::FilterProperties(std::vector<ObjectProperty*>& properties) {

    ArrangeProperties(properties, {
        L"Position",
        L"Size",
        L"Left",
        L"Top",
        L"Right",
        L"Bottom",
    });
}

}