#include <zaf/rx/internal/observable/never_observable.h>

namespace zaf::rx::internal {

const std::shared_ptr<NeverObservable>& NeverObservable::Instance() {

    static std::shared_ptr<NeverObservable> instance{ new NeverObservable{} };
    return instance;
}


std::shared_ptr<Disposable> NeverObservable::Subscribe(ObserverShim&& observer) {
    return nullptr;
}

}