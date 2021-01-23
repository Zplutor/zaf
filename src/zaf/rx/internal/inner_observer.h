#pragma once

#include <any>
#include <memory>
#include <zaf/base/error/error.h>
#include <zaf/rx/observer_functions.h>

namespace zaf::internal {

class InnerObserver {
public:
    static std::shared_ptr<InnerObserver> Create(
        zaf::OnNext<std::any> on_next,
        zaf::OnError on_error,
        zaf::OnCompleted on_completed);

public:
    InnerObserver() = default;
    virtual ~InnerObserver() = default;

    InnerObserver(const InnerObserver&) = delete;
    InnerObserver& operator=(const InnerObserver&) = delete;

    virtual void OnNext(const std::any& value) = 0;
    virtual void OnError(const Error& error) = 0;
    virtual void OnCompleted() = 0;
};

}