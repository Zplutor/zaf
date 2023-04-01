#include <zaf/rx/internal/subject/subject_core.h>
#include <algorithm>
#include <zaf/base/container/utility/erase.h>

namespace zaf::internal {
namespace {

class SubjectSubscriptionCore : public SubscriptionCore {
public:
    SubjectSubscriptionCore(
        std::shared_ptr<InnerObserver> observer,
        std::weak_ptr<SubjectCore> subject_core) 
        : 
        SubscriptionCore(std::move(observer)),
        subject_core_(std::move(subject_core)) {
    
    }

protected:
    void OnUnsubscribe() override {

        auto subject_core = subject_core_.lock();
        if (subject_core) {
            subject_core->Unsubscribe(this);
        }
    }

private:
    std::weak_ptr<SubjectCore> subject_core_;
};

}

SubjectCore::~SubjectCore() {

    for (const auto& each_core : subscription_cores_) {
        each_core->FinishSubscription();
    }
}


std::shared_ptr<SubscriptionCore> SubjectCore::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    auto subscription_core = std::make_shared<SubjectSubscriptionCore>(
        observer,
        std::dynamic_pointer_cast<SubjectCore>(shared_from_this()));
   
    subscription_cores_.push_back(subscription_core);
    return subscription_core;
}


void SubjectCore::OnNext(const std::any& value) {

    auto copied_subscription_cores = subscription_cores_;
    for (const auto& each_core : copied_subscription_cores) {
        each_core->DeliverOnNext(value);
    }
}


void SubjectCore::OnError(const Error& error) {

    auto copied_subscription_cores = subscription_cores_;
    for (const auto& each_core : copied_subscription_cores) {
        each_core->DeliverOnError(error);
    }
}


void SubjectCore::OnCompleted() {

    auto copied_subscription_cores = subscription_cores_;
    for (const auto& each_core : copied_subscription_cores) {
        each_core->DeliverOnCompleted();
    }
}


void SubjectCore::Unsubscribe(SubscriptionCore* subscription_core) {

    EraseIf(subscription_cores_, [subscription_core](const auto& core) {
        return core.get() == subscription_core;
    });
}

}