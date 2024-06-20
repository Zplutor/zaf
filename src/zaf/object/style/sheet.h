#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/object/object.h>
#include <zaf/object/style/sheet_parser.h>

namespace zaf {

class Sheet : public Object, NonCopyableNonMovable {
public:
    ZAF_OBJECT;

    void Add(std::wstring property_name, std::wstring property_value);

    void Apply(Object& object);

private:
    std::map<std::wstring, std::wstring> properties_;
};

ZAF_OBJECT_BEGIN(Sheet);
ZAF_OBJECT_PARSER(SheetParser);
ZAF_OBJECT_END;

}