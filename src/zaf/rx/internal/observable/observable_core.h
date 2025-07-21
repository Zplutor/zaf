#pragma once

#include <any>
#include <memory>
#include <zaf/rx/internal/operator/catch_handler.h>
#include <zaf/rx/internal/operator/flat_mapper.h>
#include <zaf/rx/internal/operator/mapper.h>
#include <zaf/rx/work.h>

namespace zaf::rx {
class Scheduler;
}

namespace zaf::rx::internal {

class ConnectableObservableCore;
class ObserverCore;
class SubscriptionCore;

class ObservableCore : public std::enable_shared_from_this<ObservableCore> {
public:
    ObservableCore() = default;
    virtual ~ObservableCore() = default;

    ObservableCore(const ObservableCore&) = delete;
    ObservableCore& operator=(const ObservableCore&) = delete;

    virtual std::shared_ptr<SubscriptionCore> Subscribe(
        const std::shared_ptr<ObserverCore>& observer) = 0;

    std::shared_ptr<ObservableCore> SubscribeOn(std::shared_ptr<Scheduler> scheduler);
    std::shared_ptr<ObservableCore> ObserveOn(std::shared_ptr<Scheduler> scheduler);

    std::shared_ptr<ObservableCore> Do(std::shared_ptr<ObserverCore> do_observer);
    std::shared_ptr<ObservableCore> DoOnTerminate(Work work);
    std::shared_ptr<ObservableCore> DoAfterTerminate(Work work);
    std::shared_ptr<ObservableCore> Catch(CatchHandler handler);
    std::shared_ptr<ObservableCore> Finally(Work work);

    std::shared_ptr<ObservableCore> Map(Mapper mapper);
    std::shared_ptr<ObservableCore> FlatMap(FlatMapper mapper);

    std::shared_ptr<ConnectableObservableCore> Publish();
    std::shared_ptr<ConnectableObservableCore> Replay(std::optional<std::size_t> replay_size);
};

}