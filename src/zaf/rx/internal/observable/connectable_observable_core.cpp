#include <zaf/rx/internal/observable/connectable_observable_core.h>
#include <zaf/rx/internal/subject/subject_core.h>

namespace zaf::rx::internal {

ConnectableObservableCore::ConnectableObservableCore(
    std::shared_ptr<ObservableCore> source, 
    std::shared_ptr<SubjectCore> subject) noexcept 
    :
    source_(std::move(source)),
    subject_(std::move(subject)) {

}


std::shared_ptr<SubscriptionCore> ConnectableObservableCore::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    return subject_->Subscribe(observer);
}


std::shared_ptr<SubscriptionCore> ConnectableObservableCore::Connect() {
    return source_->Subscribe(subject_);
}

}