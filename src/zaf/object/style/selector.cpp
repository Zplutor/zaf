#include <zaf/object/style/selector.h>
#include <zaf/base/error/contract_error.h>

namespace zaf {

ZAF_OBJECT_IMPL(Selector);

Selector::Selector() : sheet_(std::make_shared<zaf::Sheet>()) {

}


void Selector::AddDeclaration(ObjectProperty* property, std::shared_ptr<Object> value) {
    declarations_.emplace(property, std::move(value));
}


void Selector::AddDeclaration(std::wstring property_name, std::wstring value) {
    declarations_.emplace(std::move(property_name), std::move(value));
}


void Selector::SetSheet(std::shared_ptr<zaf::Sheet> sheet) {
    ZAF_EXPECT(sheet);
    sheet_ = std::move(sheet);
}


void Selector::ApplyTo(Object& object) const {

    if (ShouldApplySheet(object)) {
        sheet_->ApplyTo(object);
    }
}


bool Selector::ShouldApplySheet(const Object& object) const {

    for (const auto& each_declaration : declarations_) {

        if (!each_declaration.IsMatchedIn(object)) {
            return false;
        }
    }

    return true;
}

}