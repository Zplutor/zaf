#pragma once

#include <any>
#include <variant>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/observer_functions.h>

namespace zaf::rx::internal {

/**
A shim that holds either a shared pointer or a weak pointer to an ObserverCore, aiming to break
the circular reference between Producer and ObserverCore.
*/
class ObserverShim {
public:
    static ObserverShim FromShared(std::shared_ptr<ObserverCore> observer_core);
    static ObserverShim FromWeak(std::weak_ptr<ObserverCore> observer_core);

    static ObserverShim MakeShared(rx::OnNext<std::any>, rx::OnError, rx::OnCompleted);

public:
    ObserverShim() noexcept = default;

    void OnNext(const std::any& value);
    void OnError(const std::exception_ptr& error);
    void OnCompleted();

private:
    explicit ObserverShim(std::shared_ptr<ObserverCore>) noexcept;
    explicit ObserverShim(std::weak_ptr<ObserverCore>) noexcept;

private:
    std::variant<std::shared_ptr<ObserverCore>, std::weak_ptr<ObserverCore>> observer_core_;
};

}