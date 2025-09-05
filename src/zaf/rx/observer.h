#pragma once

/**
@file
    Defines the `zaf::rx::Observer<>` class template.
*/

#include <memory>
#include <zaf/base/error/contract_error.h>
#include <zaf/rx/internal/observer_core.h>

namespace zaf::rx::internal {
class ObserverInsider;
}

namespace zaf::rx {

/**
Represents an observer that receives emissions from an observable.

@tparam T
    The type of values that the observer receives.
*/
template<typename T>
class Observer {
public:
    /**
    Creates an empty observer that ignores all emissions.

    @return
        An empty observer that does nothing when it receives emissions.

    @throw std::bad_alloc
    */
    static Observer Create() {
        return Create(nullptr, nullptr, nullptr);
    }

    /**
    Creates an observer with the specified item handler, ignoring the error and completion 
    emissions.

    @param on_next
        The handler to be invoked when an item is emitted.

    @pre
        The handler is not null.

    @return
        An observer that invokes the specified item handler for each item emission.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    static Observer Create(OnNext<T> on_next) {
        ZAF_EXPECT(on_next);
        return Create(std::move(on_next), nullptr, nullptr);
    }

    /**
    Creates an observer with the specified item handler and error handler, ignoring the completion
    emission.

    @param on_next
        The handler to be invoked when an item is emitted.

    @param on_error
        The handler to be invoked when an error is emitted.

    @pre
        - The item handler is not null.
        - The error handler is not null.

    @return
        An observer that invokes the specified item handler for each item emission and the 
        specified error handler for the error emission.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    static Observer Create(OnNext<T> on_next, OnError on_error) {
        ZAF_EXPECT(on_next);
        ZAF_EXPECT(on_error);
        return Create(std::move(on_next), std::move(on_error), nullptr);
    }

    /**
    Creates an observer with the specified item handler and completion handler, ignoring the error
    emission.

    @param on_next
        The handler to be invoked when an item is emitted.

    @param on_completed
        The handler to be invoked when the completion is emitted.

    @pre
        - The item handler is not null.
        - The completion handler is not null.

    @return
        An observer that invokes the specified item handler for each item emission and the
        specified completion handler for the completion emission.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    static Observer Create(OnNext<T> on_next, OnCompleted on_completed) {
        ZAF_EXPECT(on_next);
        ZAF_EXPECT(on_completed);
        return Create(std::move(on_next), nullptr, std::move(on_completed));
    }

    /**
    Creates an observer with the specified item handler, error handler, and completion handler.

    @param on_next
        The handler to be invoked when an item is emitted.

    @param on_error
        The handler to be invoked when an error is emitted.

    @param on_completed
        The handler to be invoked when the completion is emitted.

    @return
        An observer that invokes the specified handlers for all emissions.

    @throw std::bad_alloc

    @details
        All handlers can be null, in which case the emission will be ignored. 
    */
    static Observer Create(OnNext<T> on_next, OnError on_error, OnCompleted on_completed) {

        auto bridged_on_next = [on_next](const std::any& value) {
            if (on_next) {
                on_next(std::any_cast<T>(value));
            }
        };

        return Observer(rx::internal::ObserverCore::Create(
            std::move(bridged_on_next), 
            std::move(on_error), 
            std::move(on_completed)));
    }

    /**
    Creates an observer with the specified error handler, ignoring the item and completion
    emissions.

    @param on_error
        The handler to be invoked when an error is emitted.

    @pre
        The handler is not null.

    @return
        An observer that invokes the specified error handler for the error emission.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    static Observer Create(OnError on_error) {
        ZAF_EXPECT(on_error);
        return Observer(rx::internal::ObserverCore::Create(nullptr, std::move(on_error), nullptr));
    }

public:
    void OnNext(const T& value) const {
        core_->OnNext(value);
    }

    void OnError(const std::exception_ptr& error) const {
        core_->OnError(error);
    }

    template<typename E>
    void OnError(E error) const {
        this->OnError(std::make_exception_ptr(std::move(error)));
    }

    void OnCompleted() const {
        core_->OnCompleted();
    }

protected:
    explicit Observer(std::shared_ptr<rx::internal::ObserverCore> core) noexcept :
        core_(std::move(core)) {
    }

    const std::shared_ptr<rx::internal::ObserverCore>& Core() const noexcept {
        return core_;
    }

private:
    friend class rx::internal::ObserverInsider;

private:
    std::shared_ptr<rx::internal::ObserverCore> core_;
};

}