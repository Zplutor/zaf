#include <zaf/rx/internal/subject/inner_subject.h>
#include <algorithm>
#include <zaf/base/container/utility/erase.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>

namespace zaf::internal {
namespace {

class SubjectProducer : public Producer {
public:
    SubjectProducer(
        std::shared_ptr<InnerObserver> observer,
        std::weak_ptr<InnerSubject> subject) 
        : 
        Producer(std::move(observer)),
        subject_(std::move(subject)) {
    
    }

protected:
    void OnDispose() override {

        auto subject_core = subject_.lock();
        if (subject_core) {
            subject_core->Unsubscribe(this);
        }
    }

private:
    std::weak_ptr<InnerSubject> subject_;
};

}

InnerSubject::~InnerSubject() {

    //TODO: This might be unnecessarily.
    //for (const auto& each_producer : producers_) {
        //each_producer->Dispose();
    //}
}


std::shared_ptr<InnerSubscription> InnerSubject::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    auto producer = std::make_shared<SubjectProducer>(
        observer,
        std::dynamic_pointer_cast<InnerSubject>(shared_from_this()));
   
    producers_.push_back(producer);

    return std::make_shared<InnerSubscription>(producer);
}


void InnerSubject::OnNext(const std::any& value) {

    auto copied_producers = producers_;
    for (const auto& producer : copied_producers) {
        producer->DeliverOnNext(value);
    }
}


void InnerSubject::OnError(const Error& error) {

    auto copied_producers = producers_;
    for (const auto& each_producer : copied_producers) {
        each_producer->DeliverOnError(error);
    }
}


void InnerSubject::OnCompleted() {

    auto copied_producers = producers_;
    for (const auto& each_producer : copied_producers) {
        each_producer->DeliverOnCompleted();
    }
}


void InnerSubject::Unsubscribe(Producer* unsubscribed_producer) {

    EraseIf(producers_, [unsubscribed_producer](const auto& producer) {
        return producer.get() == unsubscribed_producer;
    });
}

}