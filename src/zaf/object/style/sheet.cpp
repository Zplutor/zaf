#include <zaf/object/style/sheet.h>
#include <zaf/object/type_definition.h>
#include <zaf/object/style/sheet_parser.h>

namespace zaf {

ZAF_DEFINE_TYPE(Sheet)
ZAF_DEFINE_TYPE_PARSER(SheetParser);
ZAF_DEFINE_TYPE_END;


void Sheet::Add(std::wstring property_name, std::wstring property_value) {
    properties_.emplace(std::move(property_name), std::move(property_value));
}


void Sheet::Apply(Object& object) {

    auto type = object.GetType();

    for (const auto& [name, value] : properties_) {

        auto property = type->GetProperty(name);
        if (!property) {
            continue;
        }

        auto property_type = property->GetValueType();
        auto property_value = property_type->CreateInstance();
        property_type->GetParser()->ParseFromAttribute(value, *property_value);
        property->SetValue(object, property_value);
    }
}

}