#include <zaf/control/property_grid/textual_control_config.h>
#include <zaf/control/textual_control.h>

namespace zaf::property_grid {

void TextualControlConfig::FilterProperties(PropertyTable& property_table) {

    __super::FilterProperties(property_table);

    auto type = TextualControl::Type::Instance();

    auto list = property_table.GetList(type);
    if (!list) {
        return;
    }

    list->Erase({
        type->FontFamilyProperty,
        type->FontSizeProperty,
        type->FontWeightProperty,
    });

    list->Sort({
        type->ParagraphAlignmentProperty,
        type->TextAlignmentProperty,
        type->WordWrappingProperty,
        type->FontProperty,
        type->TextColorProperty,
        type->TextProperty,
        type->TextLengthProperty,
    });
}

}