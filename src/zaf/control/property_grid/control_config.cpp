#include <zaf/control/property_grid/control_config.h>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/erase.h>

namespace zaf::property_grid {

void ControlConfig::FilterProperties(std::vector<ObjectProperty*>& properties) {

    std::set<std::wstring> erased_properties{
        L"Parent",
        L"Window",
        L"X",
        L"Y",
        L"Position",
        L"Size",
    };

    EraseIf(properties, [&erased_properties](auto property) {
        return Contain(erased_properties, property->GetName());
    });
}

}