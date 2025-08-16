#include <zaf/rx/internal/observable/throw_observable.h>
#include <zaf/rx/disposable.h>
#include <zaf/rx/internal/observer_core.h>

namespace zaf::rx::internal {

ThrowObservable::ThrowObservable(std::exception_ptr error) : error_(std::move(error)) {

}


std::shared_ptr<Disposable> ThrowObservable::Subscribe(ObserverShim&& observer) {

    observer.OnError(error_);
    return Disposable::Empty();
}

}