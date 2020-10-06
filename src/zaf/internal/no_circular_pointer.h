#pragma once

#include <memory>
#include <zaf/control/control.h>

namespace zaf::internal {

template<typename PointerType>
class NoCircularPointer {
public:
    NoCircularPointer(Control* host_control) : host_control_(host_control) { }

    NoCircularPointer(const NoCircularPointer&) = default;
    NoCircularPointer& operator=(const NoCircularPointer&) = default;

    NoCircularPointer& operator=(const std::shared_ptr<PointerType>& pointer) {

        if (dynamic_cast<Control*>(pointer.get()) == host_control_) {
            weak_pointer_ = pointer;
            pointer_.reset();
        }
        else {
            pointer_ = pointer;
            weak_pointer_.reset();
        }
        return *this;
    }

    std::shared_ptr<PointerType> operator->() const {
        return GetSharedPointer();
    }

    std::shared_ptr<PointerType> GetSharedPointer() const {

        if (pointer_) {
            return pointer_;
        }
        else {
            return weak_pointer_.lock();
        }
    }

private:
    Control* host_control_{};
    std::shared_ptr<PointerType> pointer_;
    std::weak_ptr<PointerType> weak_pointer_;
};

}