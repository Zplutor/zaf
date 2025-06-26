#include <zaf/rx/internal/subject/subject_core.h>
#include <algorithm>
#include <zaf/base/container/utility/erase.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>

namespace zaf::rx::internal {
namespace {

class SubjectProducer : public Producer {
public:
    SubjectProducer(
        std::shared_ptr<ObserverCore> observer,
        std::weak_ptr<SubjectCore> subject) 
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
    std::weak_ptr<SubjectCore> subject_;
};

}

SubjectCore::~SubjectCore() {

}


std::shared_ptr<InnerSubscription> SubjectCore::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    auto producer = std::make_shared<SubjectProducer>(
        observer,
        std::dynamic_pointer_cast<SubjectCore>(shared_from_this()));
   
    producers_.push_back(producer);

    return std::make_shared<InnerSubscription>(producer);
}


void SubjectCore::OnNext(const std::any& value) {

    auto copied_producers = producers_;
    for (const auto& producer : copied_producers) {
        producer->EmitOnNext(value);
    }
}


void SubjectCore::OnError(const std::exception_ptr& error) {

    auto copied_producers = producers_;
    for (const auto& each_producer : copied_producers) {
        each_producer->EmitOnError(error);
    }
}


void SubjectCore::OnCompleted() {

    auto copied_producers = producers_;
    for (const auto& each_producer : copied_producers) {
        each_producer->EmitOnCompleted();
    }
}


void SubjectCore::Unsubscribe(Producer* unsubscribed_producer) {

    EraseIf(producers_, [unsubscribed_producer](const auto& producer) {
        return producer.get() == unsubscribed_producer;
    });
}

}