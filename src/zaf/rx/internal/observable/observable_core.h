#pragma once

#include <any>
#include <memory>
#include <optional>
#include <zaf/base/closure.h>
#include <zaf/rx/internal/observer_shim.h>
#include <zaf/rx/internal/operator/catch_handler.h>
#include <zaf/rx/internal/operator/flat_mapper.h>
#include <zaf/rx/internal/operator/mapper.h>

namespace zaf::rx {
class Disposable;
class DisposableHost;
class DisposeBag;
class Scheduler;
}

namespace zaf::rx::internal {

class ConnectableObservableCore;
class ObserverCore;

class ObservableCore : public std::enable_shared_from_this<ObservableCore> {
public:
    ObservableCore() = default;
    virtual ~ObservableCore() = default;

    ObservableCore(const ObservableCore&) = delete;
    ObservableCore& operator=(const ObservableCore&) = delete;

    virtual std::shared_ptr<Disposable> Subscribe(ObserverShim&& observer) = 0;

    std::shared_ptr<ObservableCore> SubscribeOn(std::shared_ptr<Scheduler> scheduler);
    std::shared_ptr<ObservableCore> ObserveOn(std::shared_ptr<Scheduler> scheduler);

    std::shared_ptr<ObservableCore> Do(std::shared_ptr<ObserverCore> do_observer);
    std::shared_ptr<ObservableCore> DoOnTerminate(Closure work);
    std::shared_ptr<ObservableCore> DoAfterTerminate(Closure work);
    std::shared_ptr<ObservableCore> Catch(CatchHandler handler);
    std::shared_ptr<ObservableCore> Finally(Closure work);

    std::shared_ptr<ObservableCore> Map(Mapper mapper);
    std::shared_ptr<ObservableCore> FlatMap(FlatMapper mapper);

    std::shared_ptr<ObservableCore> DisposeWith(DisposeBag& dispose_bag);
    std::shared_ptr<ObservableCore> DisposeWith(DisposableHost& disposable_host);

    std::shared_ptr<ConnectableObservableCore> Publish();
    std::shared_ptr<ConnectableObservableCore> Replay(std::optional<std::size_t> replay_size);
};

}