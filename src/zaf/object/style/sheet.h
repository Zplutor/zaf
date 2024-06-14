#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/object/object.h>

namespace zaf {

class Sheet : public Object, NonCopyableNonMovable {
public:
    ZAF_DECLARE_TYPE;

    void Add(std::wstring property_name, std::wstring property_value);

    void Apply(Object& object);

private:
    std::map<std::wstring, std::wstring> properties_;
};

}