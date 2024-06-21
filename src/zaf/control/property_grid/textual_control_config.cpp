#include <zaf/control/property_grid/textual_control_config.h>
#include <zaf/control/textual_control.h>

namespace zaf::property_grid {

void TextualControlConfig::FilterProperties(PropertyTable& property_table) {

    __super::FilterProperties(property_table);

    auto list = property_table.GetList(TextualControl::StaticType());
    if (!list) {
        return;
    }

    list->EraseByNames({
        L"FontFamily",
        L"FontSize",
        L"FontWeight",
    });

    list->SortByNames({
        L"ParagraphAlignment",
        L"TextAlignment",
        L"WordWrapping",
        L"Font",
        L"TextColor",
        L"Text",
        L"TextLength",
    });
}

}