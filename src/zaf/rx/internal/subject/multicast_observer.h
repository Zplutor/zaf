#pragma once

#include <optional>
#include <variant>
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
    // Unset if not terminated; std::monostate for normal termination;
    // std::exception_ptr for error termination.
    std::optional<std::variant<std::monostate, std::exception_ptr>> termination_;
};

}