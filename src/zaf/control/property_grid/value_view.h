#pragma once

#include <zaf/control/control.h>
#include <zaf/rx/subject.h>

namespace zaf::property_grid {

enum class AccessMethod {
    ReadWrite,
    ReadOnly,
};

class ValueView : public Control {
public:
    virtual void SetAccessMethod(AccessMethod) = 0;
    virtual void SetValue(const std::shared_ptr<Object>& object) = 0;

    Observable<std::shared_ptr<Object>> ValueChangedEvent() {
        return value_changed_subject_.GetObservable();
    }

protected:
    void Initialize() override;

    void NotifyValueChanged(const std::shared_ptr<Object>& new_value) {
        value_changed_subject_.GetObserver().OnNext(new_value);
    }

private:
    Subject<std::shared_ptr<Object>> value_changed_subject_;
};

}