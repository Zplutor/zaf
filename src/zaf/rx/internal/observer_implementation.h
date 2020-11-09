#pragma once

#include <any>
#include <memory>
#include <zaf/base/error/error.h>
#include <zaf/rx/observer_functions.h>

namespace zaf::internal {

class ObserverImplementation {
public:
    static std::shared_ptr<ObserverImplementation> Create(
        zaf::OnNext<std::any> on_next,
        zaf::OnError on_error,
        zaf::OnCompleted on_completed);

public:
    ObserverImplementation() = default;
    virtual ~ObserverImplementation() = default;

    ObserverImplementation(const ObserverImplementation&) = delete;
    ObserverImplementation& operator=(const ObserverImplementation&) = delete;

    virtual void OnNext(const std::any& value) = 0;
    virtual void OnError(const Error& error) = 0;
    virtual void OnCompleted() = 0;
};

}