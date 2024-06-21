#include <zaf/control/property_grid/control_config.h>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/erase.h>
#include <zaf/control/control.h>

namespace zaf::property_grid {

void ControlConfig::FilterProperties(PropertyTable& property_table) {

    auto list = property_table.GetList(Control::StaticType());
    if (!list) {
        return;
    }

    list->EraseByNames({
        L"Parent",
        L"Window",
        L"X",
        L"Y",
        L"Position",
        L"Size",
        L"Width",
        L"Height",
        L"ContentSize",
    });

    list->SortByNames({
        L"Name",
        L"AbsoluteRect",
        L"Rect",
        L"Border",
        L"Padding",
        L"Margin",
        L"ContentRect",
        L"MinWidth",
        L"MaxWidth",
        L"MinHeight",
        L"MaxHeight",
        L"AutoWidth",
        L"AutoHeight",
    });
}

}