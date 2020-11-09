#include <zaf/rx/internal/subject/subject_core.h>
#include <algorithm>
#include <zaf/base/container/utility/erase.h>
#include <zaf/rx/internal/subscription/subscription_implementation.h>

namespace zaf::internal {
namespace {

class SubjectSubscriptionCore : public SubscriptionCore {
public:
    SubjectSubscriptionCore(
        std::shared_ptr<ObserverImplementation> observer,
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


std::shared_ptr<SubscriptionImplementation> SubjectCore::Subscribe(
    const std::shared_ptr<ObserverImplementation>& observer) {

    auto subscription_core = std::make_shared<SubjectSubscriptionCore>(
        observer,
        shared_from_this());
   
    subscription_cores_.push_back(subscription_core);
    
    return std::make_shared<SubscriptionImplementation>(subscription_core);
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