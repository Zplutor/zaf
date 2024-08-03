#pragma once

#include <zaf/control/style/if_parser.h>
#include <zaf/control/style/property_value.h>
#include <zaf/control/style/set.h>
#include <zaf/control/style/style.h>

namespace zaf {

class If : public Style {
public:
    ZAF_OBJECT;

    void AddCondition(ObjectProperty* property, std::shared_ptr<Object> value);
    void AddCondition(std::wstring property_name, std::wstring value);

    void SetResult(std::shared_ptr<Set> result);

    void ApplyTo(Object& object) override;

private:
    bool ShouldApply(const Object& object) const;

private:
    PropertyValueSet conditions_;
    std::shared_ptr<Set> result_;
};

ZAF_OBJECT_BEGIN(If);
ZAF_OBJECT_PARSER(IfParser);
ZAF_OBJECT_END;

}