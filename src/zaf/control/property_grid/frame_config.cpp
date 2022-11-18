#include <zaf/control/property_grid/frame_config.h>

namespace zaf::property_grid {

void FrameConfig::FilterProperties(std::vector<ObjectProperty*>& properties) {

    ArrangeProperties(properties, {
        L"Left",
        L"Top",
        L"Right",
        L"Bottom",
    });
}

}