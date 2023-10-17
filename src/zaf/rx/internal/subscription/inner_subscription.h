#pragma once

#include <memory>
#include <zaf/base/non_copyable.h>

namespace zaf::internal {

class Producer;

class InnerSubscription : NonCopyableNonMovable {
public:
    static std::shared_ptr<InnerSubscription> Empty();

public:
    InnerSubscription(std::shared_ptr<internal::Producer> producer);
    ~InnerSubscription();

    void Unsubscribe();

    const std::shared_ptr<Producer>& Producer() {
        return producer_;
    }

private:
    std::shared_ptr<internal::Producer> producer_;
};

}