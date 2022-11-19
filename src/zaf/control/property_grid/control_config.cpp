#include <zaf/control/property_grid/control_config.h>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/erase.h>
#include <zaf/control/control.h>

namespace zaf::property_grid {

void ControlConfig::FilterProperties(PropertyTable& property_table) {

    auto list = property_table.GetList(Control::Type);
    if (list) {
        list->EraseByNames({
            L"Parent",
            L"Window",
            L"X",
            L"Y",
            L"Position",
            L"Size",
        });
    }
}

}