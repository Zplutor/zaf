#pragma once

#include <zaf/base/error/check.h>

namespace zaf {

class Control;
class Window;

namespace internal {

std::shared_ptr<Control> BridgedFindControl(const Control& control, const wchar_t* name);
std::shared_ptr<Control> BridgedFindControl(const Window& window, const wchar_t* name);

template<typename T>
class ControlBinder {
public:
    ControlBinder(const Control* owner, const wchar_t* name) : 
        owner_is_window_(false),
        name_(name) {

        ZAF_EXPECT(owner);
        owner_.control = owner;
    }

    ControlBinder(const Window* owner, const wchar_t* name) :
        owner_is_window_(true),
        name_(name) {

        ZAF_EXPECT(owner);
        owner_.window = owner;
    }

    ControlBinder(const ControlBinder&) = delete;
    ControlBinder& operator=(const ControlBinder&) = delete;

    T* operator->() {

        auto control = control_.lock();
        if (control) {
            return control.get();
        }

        control = FindControl();
        if (control) {
            control_ = control;
            return control.get();
        }

        throw zaf::Error{ {} };
    }

private:
    std::shared_ptr<T> FindControl() const {

        std::shared_ptr<Control> control;
        if (owner_is_window_) {
            control = BridgedFindControl(*owner_.window, name_);
        }
        else {
            control = BridgedFindControl(*owner_.control, name_);
        }
        return std::dynamic_pointer_cast<T>(control);
    }

private:
    union {
        const Control* control;
        const Window* window;
    } owner_{};
    bool owner_is_window_{};
    const wchar_t* name_;
    std::weak_ptr<T> control_;
};

}

#define ZAF_BIND_CONTROL(Type, ControlName) \
zaf::internal::ControlBinder<Type> ControlName{ this, L#ControlName }

}