#include <zaf/rx/internal/subject/inner_subject.h>
#include <algorithm>
#include <zaf/base/container/utility/erase.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>

namespace zaf::internal {
namespace {

class SubjectSubscriptionCore : public Producer {
public:
    SubjectSubscriptionCore(
        std::shared_ptr<InnerObserver> observer,
        std::weak_ptr<InnerSubject> subject_core) 
        : 
        Producer(std::move(observer)),
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
    std::weak_ptr<InnerSubject> subject_core_;
};

}

InnerSubject::~InnerSubject() {

    for (const auto& each_core : subscription_cores_) {
        each_core->FinishSubscription();
    }
}


std::shared_ptr<InnerSubscription> InnerSubject::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    auto subscription_core = std::make_shared<SubjectSubscriptionCore>(
        observer,
        std::dynamic_pointer_cast<InnerSubject>(shared_from_this()));
   
    subscription_cores_.push_back(subscription_core);

    return std::make_shared<InnerSubscription>(subscription_core);
}


void InnerSubject::OnNext(const std::any& value) {

    auto copied_subscription_cores = subscription_cores_;
    for (const auto& each_core : copied_subscription_cores) {
        each_core->DeliverOnNext(value);
    }
}


void InnerSubject::OnError(const Error& error) {

    auto copied_subscription_cores = subscription_cores_;
    for (const auto& each_core : copied_subscription_cores) {
        each_core->DeliverOnError(error);
    }
}


void InnerSubject::OnCompleted() {

    auto copied_subscription_cores = subscription_cores_;
    for (const auto& each_core : copied_subscription_cores) {
        each_core->DeliverOnCompleted();
    }
}


void InnerSubject::Unsubscribe(Producer* subscription_core) {

    EraseIf(subscription_cores_, [subscription_core](const auto& core) {
        return core.get() == subscription_core;
    });
}

}