#pragma once

#include <any>
#include <memory>

namespace zaf {

class Scheduler;

namespace internal {

class InnerObserver;
class InnerSubscription;

class InnerObservable : public std::enable_shared_from_this<InnerObservable> {
public:
    InnerObservable() = default;
    virtual ~InnerObservable() = default;

    InnerObservable(const InnerObservable&) = delete;
    InnerObservable& operator=(const InnerObservable&) = delete;

    virtual std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) = 0;

    std::shared_ptr<InnerObservable> SubscribeOn(std::shared_ptr<Scheduler> scheduler);
    std::shared_ptr<InnerObservable> ObserveOn(std::shared_ptr<Scheduler> scheculer);
};

}
}