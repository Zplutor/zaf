#pragma once

/**
@file
    Defines the `zaf::rx::DisposableHost` class.
*/

#include <zaf/rx/dispose_bag.h>

namespace zaf::rx {

/**
An utility base class that provides a `zaf::rx::DisposeBag` instance for derived classes to store 
disposables that should be disposed when the derived class instance is destructed.
*/
class DisposableHost {
public:
    DisposableHost() = default;
    ~DisposableHost() = default;

    DisposableHost(const DisposableHost&) = delete;
    DisposableHost& operator=(const DisposableHost&) = delete;

    /**
    Gets the `zaf::rx::DisposeBag` instance.
    */
    DisposeBag& Disposables() noexcept {
        return dispose_bag_;
    }

private:
    DisposeBag dispose_bag_;
};

}