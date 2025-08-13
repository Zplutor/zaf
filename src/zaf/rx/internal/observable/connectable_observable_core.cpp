#include <zaf/rx/internal/observable/connectable_observable_core.h>
#include <zaf/rx/internal/operator/ref_count_operator.h>
#include <zaf/rx/internal/subject/subject_core.h>

namespace zaf::rx::internal {

ConnectableObservableCore::ConnectableObservableCore(
    std::shared_ptr<ObservableCore> source, 
    std::shared_ptr<SubjectCore> subject) noexcept 
    :
    source_(std::move(source)),
    subject_(std::move(subject)) {

}


std::shared_ptr<SubscriptionCore> ConnectableObservableCore::Subscribe(ObserverShim&& observer) {
    return subject_->Subscribe(std::move(observer));
}


std::shared_ptr<SubscriptionCore> ConnectableObservableCore::Connect() {
    return source_->Subscribe(ObserverShim::FromShared(subject_));
}


std::shared_ptr<ObservableCore> ConnectableObservableCore::RefCount() {
    auto source = std::dynamic_pointer_cast<ConnectableObservableCore>(shared_from_this());
    return std::make_shared<RefCountOperator>(std::move(source));
}

}