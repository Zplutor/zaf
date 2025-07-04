#pragma once

#include <memory>
#include <zaf/base/non_copyable.h>

namespace zaf::rx::internal {

class Producer;

class SubscriptionCore : NonCopyableNonMovable {
public:
    SubscriptionCore(std::shared_ptr<internal::Producer> producer);
    ~SubscriptionCore();

    void Unsubscribe();

    const std::shared_ptr<Producer>& Producer() {
        return producer_;
    }

private:
    std::shared_ptr<internal::Producer> producer_;
};

}