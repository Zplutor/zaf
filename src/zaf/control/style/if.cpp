#include <zaf/control/style/if.h>
#include <zaf/base/error/contract_error.h>

namespace zaf {

ZAF_OBJECT_IMPL(If);

If::If() : result_(Create<Set>()) {

}


void If::AddCondition(ObjectProperty* property, std::shared_ptr<Object> value) {
    conditions_.emplace(property, std::move(value));
}


void If::AddCondition(std::wstring property_name, std::wstring value) {
    conditions_.emplace(std::move(property_name), std::move(value));
}


void If::SetResult(std::shared_ptr<Set> result) {
    ZAF_EXPECT(result);
    result_ = std::move(result);
}


void If::ApplyTo(Object& object) {

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