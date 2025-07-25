#pragma once

#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/internal/subject/multicast_observer.h>

namespace zaf::rx::internal {

class SubjectCore : public ObservableCore, public ObserverCore {
public:
    SubjectCore();
    ~SubjectCore();

    std::shared_ptr<SubscriptionCore> Subscribe(
        const std::shared_ptr<ObserverCore>& observer) override;

    void OnNext(const std::any& value) override;
    void OnError(const std::exception_ptr& error) override;
    void OnCompleted() override;

    //For unit test.
    std::size_t SubscriptionCount() const noexcept;

protected:
    bool TryEmitOnNext(const std::any& value);

private:
    std::shared_ptr<MulticastObserver> multicast_observer_;
    std::shared_ptr<Producer> producer_;
};

}