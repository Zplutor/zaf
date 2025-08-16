#include <zaf/rx/internal/observable/just_observable.h>
#include <zaf/rx/disposable.h>
#include <zaf/rx/internal/observer_core.h>

namespace zaf::rx::internal {

JustObservable::JustObservable(std::any value) : value_(std::move(value)) {

}


std::shared_ptr<Disposable> JustObservable::Subscribe(ObserverShim&& observer) {

    observer.OnNext(value_);
    observer.OnCompleted();
    return Disposable::Empty();
}

}