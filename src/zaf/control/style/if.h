#pragma once

#include <zaf/control/style/if_parser.h>
#include <zaf/control/style/property_value_pair.h>
#include <zaf/dynamic/object.h>

namespace zaf {

class If : public Object {
public:
    ZAF_OBJECT;

public:
    If();

    void AddCondition(ObjectProperty* property, std::shared_ptr<Object> value);
    void AddCondition(std::wstring property_name, std::wstring value);

    const PropertyValuePairSet& Conditions() const noexcept {
        return conditions_;
    }

    void SetResult(std::shared_ptr<Object> result);

    const std::shared_ptr<Object>& Result() const noexcept {
        return result_;
    }

    std::shared_ptr<Object> Evaluate(const Object& object) const;

private:
    bool MeetsConditions(const Object& object) const;

private:
    PropertyValuePairSet conditions_;
    std::shared_ptr<Object> result_;
};

ZAF_OBJECT_BEGIN(If);
ZAF_OBJECT_PARSER(IfParser);
ZAF_OBJECT_END;

}