#pragma once

#include <any>
#include <memory>

namespace zaf::internal {

class ObserverImplementation;
class SubscriptionImplementation;

class ObservableImplementation {
public:
    ObservableImplementation() = default;
    virtual ~ObservableImplementation() = default;

    ObservableImplementation(const ObservableImplementation&) = delete;
    ObservableImplementation& operator=(const ObservableImplementation&) = delete;

    virtual std::shared_ptr<SubscriptionImplementation> Subscribe(
        const std::shared_ptr<ObserverImplementation>& observer) = 0;
};

}