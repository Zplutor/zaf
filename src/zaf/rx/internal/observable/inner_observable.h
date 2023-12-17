#pragma once

#include <any>
#include <memory>
#include <zaf/rx/internal/operator/catch_handler.h>
#include <zaf/rx/internal/operator/flat_mapper.h>
#include <zaf/rx/internal/operator/mapper.h>
#include <zaf/rx/work.h>

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

    std::shared_ptr<InnerObservable> Do(std::shared_ptr<InnerObserver> do_observer);
    std::shared_ptr<InnerObservable> DoOnTerminated(Work work);
    std::shared_ptr<InnerObservable> Catch(CatchHandler handler);
    std::shared_ptr<InnerObservable> Finally(Work work);

    std::shared_ptr<InnerObservable> Map(Mapper mapper);
    std::shared_ptr<InnerObservable> FlatMap(FlatMapper mapper);
};

}
}