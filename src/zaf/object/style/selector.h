#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/object/object.h>
#include <zaf/object/style/selector_parser.h>
#include <zaf/object/style/sheet.h>

namespace zaf {

class Selector : public Object, NonCopyableNonMovable {
public:
    ZAF_OBJECT;

    Selector();

    void AddDeclaration(ObjectProperty* property, std::shared_ptr<Object> value);
    void AddDeclaration(std::wstring property_name, std::wstring value);

    const std::shared_ptr<Sheet>& Sheet() const noexcept {
        return sheet_;
    }

    void SetSheet(std::shared_ptr<zaf::Sheet> sheet);

    void ApplyTo(Object& object) const;

private:
    bool ShouldApplySheet(const Object& object) const;

private:
    DeclarationSet declarations_;
    std::shared_ptr<zaf::Sheet> sheet_;
};

ZAF_OBJECT_BEGIN(Selector)
ZAF_OBJECT_PARSER(SelectorParser)
ZAF_OBJECT_END;

}