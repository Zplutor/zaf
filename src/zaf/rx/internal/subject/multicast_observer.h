#pragma once

#include <variant>
#include <zaf/base/none.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {

class MulticastObserver : 
    public ObserverCore, 
    public std::enable_shared_from_this<MulticastObserver> {

public:
    ~MulticastObserver();

    std::shared_ptr<Disposable> AddObserver(ObserverShim&& observer);

    std::size_t ObserverCount() const noexcept;

    void OnNext(const std::any& value) override;
    void OnError(const std::exception_ptr& error) override;
    void OnCompleted() override;
    
private:
    class IndividualProducer;

private:
    void RemoveProducer(IndividualProducer*) noexcept;

private:
    mutable std::mutex lock_;
    std::vector<std::weak_ptr<IndividualProducer>> producers_;
    //std::exception_ptr for error termination; None for normal termination.
    std::variant<std::monostate, std::exception_ptr, None> termination_;
};

}