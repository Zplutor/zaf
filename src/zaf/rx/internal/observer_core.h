#pragma once

#include <any>
#include <memory>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/observer_functions.h>

namespace zaf::rx::internal {

class ObserverCore : NonCopyableNonMovable {
public:
    static std::shared_ptr<ObserverCore> Create(
        rx::OnNext<std::any> on_next,
        rx::OnError on_error,
        rx::OnCompleted on_completed);

public:
    ObserverCore() = default;
    virtual ~ObserverCore() = default;

    virtual void OnNext(const std::any& value) = 0;
    virtual void OnError(const std::exception_ptr& error) = 0;
    virtual void OnCompleted() = 0;
};

}