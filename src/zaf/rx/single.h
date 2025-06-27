#pragma once

#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/internal/observable/just_observable.h>
#include <zaf/rx/internal/observable/never_observable.h>
#include <zaf/rx/internal/observable/throw_observable.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/single_observer.h>
#include <zaf/rx/subscription.h>

namespace zaf::rx::internal {
template<typename T>
class SingleFactory;
}

namespace zaf::rx {

template<typename T>
class Single {
public:
    static Single Just(T value) {
        return Single{ std::make_shared<internal::JustObservable>(std::any{ std::move(value) }) };
    }

    static Single Throw(std::exception_ptr error) {
        return Single{ std::make_shared<internal::ThrowObservable>(std::move(error)) };
    }

    template<typename E>
    static Single Throw(E&& error) {
        return Throw(std::make_exception_ptr(std::forward<E>(error)));
    }

    static Single Never() {
        return Single{ internal::NeverObservable::Instance() };
    }

public:
    [[nodiscard]]
    Subscription Subscribe() {
        return Subscribe(nullptr, nullptr);
    }

    [[nodiscard]]
    Subscription Subscribe(OnSuccess<T> on_success) {
        return Subscribe(std::move(on_success), nullptr);
    }

    [[nodiscard]]
    Subscription Subscribe(OnSuccess<T> on_success, OnError on_error) {
        auto observer = SingleObserver<T>::Create(std::move(on_success), std::move(on_error));
        return Subscription{ core_->Subscribe(observer.Core()) };
    }

    Single Do(OnSuccess<T> on_success) {
        return Do(SingleObserver<T>::Create(std::move(on_success), nullptr));
    }

    Single Do(OnSuccess<T> on_success, OnError on_error) {
        return Do(SingleObserver<T>::Create(std::move(on_success), std::move(on_error)));
    }

    Single Do(const SingleObserver<T>& observer) {
        return Single{ core_->Do(observer.Core()) };
    }

    Single DoOnError(OnError on_error) {
        return Do(nullptr, std::move(on_error));
    }

    template<typename E>
    Single DoOnError(std::function<void(const E&)> handler) {
        return Do(
            nullptr, 
            [handler = std::move(handler)](const std::exception_ptr& exception) {
                try {
                    std::rethrow_exception(exception);
                }
                catch (const E& error) {
                    handler(error);
                }
                catch (...) {

                }
            });
    }

    Single DoOnTerminated(Work work) {
        return Single{ core_->DoOnTerminated(std::move(work)) };
    }

    Single Finally(Work work) {
        return Single{ core_->Finally(std::move(work)) };
    }

    template<typename K>
    Single<K> Map(std::function<K(const T&)> mapper) {
        auto new_core = core_->Map([mapper = std::move(mapper)](const std::any& value) {
            return mapper(std::any_cast<T>(value));
        });
        return Single<K>{ std::move(new_core) };
    }

    template<typename K>
    Single<K> FlatMap(std::function<Single<K>(const T&)> mapper) {
        auto new_core = core_->FlatMap([mapper = std::move(mapper)](const std::any& value) {
            return mapper(std::any_cast<T>(value)).Core();
        });
        return Single<K>{ std::move(new_core) };
    }

    const std::shared_ptr<internal::ObservableCore>& Core() const noexcept {
        return core_;
    }

    /**
    Converts a single to an observable implicitly.

    @details
        We define a conversion operator in `Single<>` rather than defining a constructor in
        `Observable<>` to avoid `Observable<>` depending on `Single<>`, as `Single<>` is a
        specialized type of `Observable<>`.
    */
    operator Observable<T>() const noexcept {
        return Observable<T>{ core_ };
    }

private:
    template<typename U>
    friend class Single;

    friend class zaf::rx::internal::SingleFactory<T>;

    explicit Single(std::shared_ptr<internal::ObservableCore> core) noexcept :
        core_(std::move(core)) {

    }

private:
    std::shared_ptr<internal::ObservableCore> core_;
};

}