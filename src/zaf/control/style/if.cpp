#include <zaf/control/style/if.h>
#include <zaf/base/error/precondition_error.h>

namespace zaf {

ZAF_OBJECT_IMPL(If);

If::If() : result_(Create<Object>()) {

}


void If::AddCondition(ObjectProperty* property, std::shared_ptr<Object> value) {
    conditions_.emplace(property, std::move(value));
}


void If::AddCondition(std::wstring property_name, std::wstring value) {
    conditions_.emplace(std::move(property_name), std::move(value));
}


void If::SetResult(std::shared_ptr<Object> result) {
    ZAF_EXPECT(result);
    result_ = std::move(result);
}


std::shared_ptr<Object> If::Evaluate(const Object& object) const{
    
    if (MeetsConditions(object)) {
        return result_;
    }
    else {
        return nullptr;
    }
}


bool If::MeetsConditions(const Object& object) const {

    for (const auto& each_condition : conditions_) {

        if (!each_condition.IsSetIn(object)) {
            return false;
        }
    }

    return true;
}

}