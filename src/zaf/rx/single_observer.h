#pragma once

/**
@file
    Defines the `zaf::rx::SingleObserver<>` class template.
*/

#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/observer_functions.h>

namespace zaf::rx::internal {
class SingleObserverInsider;
}

namespace zaf::rx {

/**
The type of the handler to be invoked when a single emits the item.
*/
template<typename T>
using OnSuccess = OnNext<T>;

/**
Represents an observer that receives emissions from a `zaf::rx::Single<>`.
*/
template<typename T>
class SingleObserver {
public:
    /**
    Creates an empty observer without any handlers.

    @return
        An empty observer that does nothing when it receives emissions.

    @throw std::bad_alloc
    */
    static SingleObserver Create() {
        return SingleObserver{ internal::ObserverCore::Create(nullptr, nullptr, nullptr) };
    }

    /**
    Creates an observer with the specified success handler, ignoring the error emission.

    @param on_success
        The handler to be invoked when the single emits the item.

    @pre
        The handler is not null.

    @return
        A single observer that invokes the specified success handler for the item emission.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    static SingleObserver Create(OnSuccess<T> on_success) {
        ZAF_EXPECT(on_success);
        return Create(std::move(on_success), nullptr);
    }

    /**
    Creates an observer with the specified success handler and error handler.

    @param on_success
        The handler to be invoked when the single emits the item.

    @param on_error
        The handler to be invoked when the single emits an error.

    @return
        A single observer that invokes the specified success handler and error handler for the 
        emissions.

    @throw std::bad_alloc

    @details
        The success handler and the error handler can be null, in which case the corresponding
        emission will be ignored.
    */
    static SingleObserver Create(OnSuccess<T> on_success, OnError on_error) {

        auto on_success_bridge = [inner = std::move(on_success)](const std::any& value) {
            if (inner) {
                inner(std::any_cast<T>(value));
            }
        };

        return SingleObserver{ 
            internal::ObserverCore::Create(
                std::move(on_success_bridge), 
                std::move(on_error), 
                nullptr)
        };
    }

    /**
    Creates an observer with the specified error handler, ignoring the success emission.

    @param on_error
        The handler to be invoked when an error is emitted.

    @pre
        The handler is not null.

    @return
        A single observer that invokes the specified error handler for the error emission.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    static SingleObserver Create(OnError on_error) {
        return SingleObserver{ 
            internal::ObserverCore::Create(nullptr, std::move(on_error), nullptr) 
        };
    }   

public:
    /**
    Emits the item to the observer, invokes the success handler.

    @param value
        The item to emit.

    @throw ...
        Any exception thrown by the success handler.
    */
    void OnSuccess(const T& value) {
        core_->OnNext(value);
        core_->OnCompleted();
    }

    /**
    Emits an error to the observer, invokes the error handler.

    @param error
        The error to emit.

    @pre 
        The error is not null.

    @throw ...
        Any exception thrown by the error handler.
    */
    void OnError(const std::exception_ptr& error) {
        ZAF_EXPECT(error);
        core_->OnError(error);
    }

    /**
    Emits an error to the observer, invokes the error handler.

    @tparam E
        The type of the error to emit.

    @param error
        The error to emit.

    @throw ...
        Any exception thrown by the error handler.

    @details
        This is a convenience for creating an `std::exception_ptr` from the specified error
        and calling `OnError(const std::exception_ptr&)` overload.
    */
    template<typename E>
    void OnError(E&& error) const {
        core_->OnError(std::make_exception_ptr(std::forward<E>(error)));
    }

protected:
    explicit SingleObserver(std::shared_ptr<internal::ObserverCore> core) :
        core_(std::move(core)) {
    }

    const std::shared_ptr<internal::ObserverCore>& Core() const noexcept {
        return core_;
    }

private:
    friend class internal::SingleObserverInsider;

private:
    std::shared_ptr<internal::ObserverCore> core_;
};

}
