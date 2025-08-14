#include <zaf/rx/internal/observer_shim.h>

namespace zaf::rx::internal {

ObserverShim ObserverShim::FromShared(std::shared_ptr<ObserverCore> observer_core) {
    return ObserverShim{ std::move(observer_core) };
}


ObserverShim ObserverShim::FromWeak(std::weak_ptr<ObserverCore> observer_core) {
    return ObserverShim{ std::move(observer_core) };
}


ObserverShim ObserverShim::MakeShared(
    rx::OnNext<std::any> on_next, 
    rx::OnError on_error, 
    rx::OnCompleted on_completed) {

    auto observer_core = ObserverCore::Create(
        std::move(on_next), 
        std::move(on_error), 
        std::move(on_completed));

    return FromShared(std::move(observer_core));
}


ObserverShim::ObserverShim(std::shared_ptr<ObserverCore> observer_core) noexcept : 
    observer_core_(std::move(observer_core)) {

}


ObserverShim::ObserverShim(std::weak_ptr<ObserverCore> observer_core) noexcept :
    observer_core_(std::move(observer_core)) {

}


void ObserverShim::OnNext(const std::any& value) {

    // Note that we use `auto` rather than `const auto&` here, because we want to copy the 
    // observer_core_ pointer to retain its lifetime during the call to OnNext().
    // The same as the OnError() and OnCompleted() methods.
    std::visit([&value](auto observer_core) {

        using PointerType = std::decay_t<decltype(observer_core)>;
        if constexpr (std::is_same_v<PointerType, std::shared_ptr<ObserverCore>>) {
            observer_core->OnNext(value);
        }
        else if constexpr (std::is_same_v<PointerType, std::weak_ptr<ObserverCore>>) {
            if (auto shared = observer_core.lock()) {
                shared->OnNext(value);
            }
        }
    }, observer_core_);
}


void ObserverShim::OnError(const std::exception_ptr& error) {
    std::visit([&error](auto observer_core) {

        using PointerType = std::decay_t<decltype(observer_core)>;
        if constexpr (std::is_same_v<PointerType, std::shared_ptr<ObserverCore>>) {
            observer_core->OnError(error);
        }
        else if constexpr (std::is_same_v<PointerType, std::weak_ptr<ObserverCore>>) {
            if (auto shared = observer_core.lock()) {
                shared->OnError(error);
            }
        }
    }, observer_core_);
}


void ObserverShim::OnCompleted() {
    std::visit([](auto observer_core) {

        using PointerType = std::decay_t<decltype(observer_core)>;
        if constexpr (std::is_same_v<PointerType, std::shared_ptr<ObserverCore>>) {
            observer_core->OnCompleted();
        }
        else if constexpr (std::is_same_v<PointerType, std::weak_ptr<ObserverCore>>) {
            if (auto shared = observer_core.lock()) {
                shared->OnCompleted();
            }
        }
    }, observer_core_);
}

}