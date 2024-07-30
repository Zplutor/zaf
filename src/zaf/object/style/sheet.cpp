#include <zaf/object/style/sheet.h>

namespace zaf {

ZAF_OBJECT_IMPL(Sheet);

void Sheet::AddDeclaration(ObjectProperty* property, std::shared_ptr<Object> value) {
    declarations_.emplace_back(property, std::move(value));
}


void Sheet::AddDeclaration(std::wstring property_name, std::wstring value) {
    declarations_.emplace_back(std::move(property_name), std::move(value));
}


void Sheet::Apply(Object& object) const {

    for (const auto& each_declaration : declarations_) {
        each_declaration.Apply(object);
    }
}

}