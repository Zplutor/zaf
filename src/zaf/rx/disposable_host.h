#pragma once

#include <zaf/rx/dispose_bag.h>

namespace zaf::rx {

class DisposableHost {
public:
    DisposableHost() = default;
    ~DisposableHost() = default;

    DisposableHost(const DisposableHost&) = delete;
    DisposableHost& operator=(const DisposableHost&) = delete;

    DisposeBag& Disposables() {
        return dispose_bag_;
    }

private:
    DisposeBag dispose_bag_;
};

}