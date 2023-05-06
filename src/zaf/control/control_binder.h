#pragma once

#include <memory>
#include <zaf/base/error/basic_error.h>
#include <zaf/base/error/check.h>
#include <zaf/base/error/error.h>
#include <zaf/base/non_copyable.h>

namespace zaf {

class Control;
class Window;

namespace internal {

std::shared_ptr<Control> BridgedFindControl(const Control& control, const wchar_t* name);
std::shared_ptr<Control> BridgedFindControl(const Window& window, const wchar_t* name);

template<typename T>
class ControlBinder : NonCopyableNonMovable {
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

    T* operator->() {
        return GetBoundControl().get();
    }

    T* operator->() const {
        auto mutable_this = const_cast<ControlBinder<T>*>(this);
        return mutable_this->operator->();
    }

    operator std::shared_ptr<T>() {
        return GetBoundControl();
    }

    operator std::shared_ptr<T>() const {
        auto mutable_this = const_cast<ControlBinder<T>*>(this);
        return mutable_this->operator std::shared_ptr<T>();
    }

private:
    std::shared_ptr<T> GetBoundControl() {

        auto control = control_.lock();
        if (control) {
            return control;
        }

        control = FindControl();
        if (control) {
            control_ = control;
            return control;
        }

        ZAF_THROW_ERRC(zaf::BasicErrc::NameNotFound);
    }

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