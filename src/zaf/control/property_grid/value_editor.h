#pragma once

#include <zaf/control/control.h>
#include <zaf/rx/subject.h>

namespace zaf::property_grid {

enum class AccessMethod {
    ReadWrite,
    ReadOnly,
};

class ValueEditor : public Control {
public:
    virtual void SetAccessMethod(AccessMethod) = 0;
    virtual void SetValue(const std::shared_ptr<Object>& object) = 0;

    rx::Observable<std::shared_ptr<Object>> ValueChangedEvent() {
        return value_changed_event_.AsObservable();
    }

protected:
    void Initialize() override;

    void NotifyValueChanged(const std::shared_ptr<Object>& new_value) {
        value_changed_event_.AsObserver().OnNext(new_value);
    }

private:
    rx::Subject<std::shared_ptr<Object>> value_changed_event_;
};

}