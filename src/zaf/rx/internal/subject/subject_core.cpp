#include <zaf/rx/internal/subject/subject_core.h>

namespace zaf::rx::internal {

SubjectCore::SubjectCore() : 
    multicast_observer_(std::make_shared<MulticastObserver>()),
    producer_(std::make_shared<Producer>(multicast_observer_)) {

}


SubjectCore::~SubjectCore() {
    // The multicast observer will unsubscribe all observers when it is destroyed.
    multicast_observer_.reset();
    producer_->Unsubscribe();
}


std::shared_ptr<SubscriptionCore> SubjectCore::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    return multicast_observer_->AddObserver(observer);
}


void SubjectCore::OnNext(const std::any& value) {
    TryEmitOnNext(value);
}


bool SubjectCore::TryEmitOnNext(const std::any& value) {
    return producer_->EmitOnNext(value);
}


void SubjectCore::OnError(const std::exception_ptr& error) {
    producer_->EmitOnError(error);
}


void SubjectCore::OnCompleted() {
    producer_->EmitOnCompleted();
}


std::size_t SubjectCore::SubscriptionCount() const noexcept {
    return multicast_observer_->ObserverCount();
}

}