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

    virtual void RecoverFocus();

    Observable<std::shared_ptr<Object>> ValueChangedEvent() {
        return value_changed_event_.GetObservable();
    }

    Observable<ValueView*> ShouldSelectEvent() {
        return should_select_event_.GetObservable();
    }

protected:
    void Initialize() override;

    void NotifyValueChanged(const std::shared_ptr<Object>& new_value) {
        value_changed_event_.GetObserver().OnNext(new_value);
    }

    void NotifyShouldSelectItem() {
        should_select_event_.GetObserver().OnNext(this);
    }

private:
    Subject<std::shared_ptr<Object>> value_changed_event_;
    Subject<ValueView*> should_select_event_;
};

}