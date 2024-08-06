#include <zaf/control/style/if.h>

namespace zaf {

ZAF_OBJECT_IMPL(If);

void If::AddCondition(ObjectProperty* property, std::shared_ptr<Object> value) {
    conditions_.emplace(property, std::move(value));
}


void If::AddCondition(std::wstring property_name, std::wstring value) {
    conditions_.emplace(std::move(property_name), std::move(value));
}


void If::SetResult(std::shared_ptr<Set> result) {
    result_ = std::move(result);
}


void If::ApplyTo(Object& object) {

    if (!result_) {
        return;
    }

    if (ShouldApply(object)) {
        result_->ApplyTo(object);
    }
}


bool If::ShouldApply(const Object& object) const {

    for (const auto& each_condition : conditions_) {

        if (!each_condition.IsSetIn(object)) {
            return false;
        }
    }

    return true;
}

}