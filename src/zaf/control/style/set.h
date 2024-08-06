#pragma once

#include <zaf/control/style/property_value_pair.h>
#include <zaf/control/style/set_parser.h>
#include <zaf/control/style/style.h>

namespace zaf {

class Set : public Style {
public:
    ZAF_OBJECT;

    void AddProperty(ObjectProperty* property, std::shared_ptr<Object> value);
    void AddProperty(std::wstring property_name, std::wstring value);

    void ApplyTo(Object& object) override;

private:
    PropertyValuePairSet properties_;
};

ZAF_OBJECT_BEGIN(Set);
ZAF_OBJECT_PARSER(SetParser);
ZAF_OBJECT_END;

}