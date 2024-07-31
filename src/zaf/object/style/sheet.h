#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/object/object.h>
#include <zaf/object/style/declaration.h>
#include <zaf/object/style/sheet_parser.h>

namespace zaf {

class Sheet : public Object, NonCopyableNonMovable {
public:
    ZAF_OBJECT;

    void AddDeclaration(ObjectProperty* property, std::shared_ptr<Object> value);
    void AddDeclaration(std::wstring property_name, std::wstring value);

    void ApplyTo(Object& object) const;

private:
    DeclarationSet declarations_;
};

ZAF_OBJECT_BEGIN(Sheet);
ZAF_OBJECT_PARSER(SheetParser);
ZAF_OBJECT_END;

}