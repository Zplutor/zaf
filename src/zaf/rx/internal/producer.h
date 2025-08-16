#pragma once

#include <functional>
#include <optional>
#include <map>
#include <mutex>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/disposable.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/observer_shim.h>

namespace zaf::rx::internal {

/**
A producer is responsible for emitting data sequence.

@details
    There are two states of a producer: subscribed and disposed. Subscribed state indicates 
    that the producer is actively emitting values. While disposed state indicates that the 
    producer is no longer emitting values.

    A producer transitions from subscribed state to disposed state either when Dispose() is
    called, or when EmitOnError() or EmitOnCompleted() is called.
*/
class Producer : public Disposable, public std::enable_shared_from_this<Producer> {
public:
    explicit Producer(ObserverShim&& observer) noexcept;

    /**
    Emits a data item to the observer.

    @return
        True if the item was successfully emitted, false if the producer is already terminated.
    */
    bool EmitOnNext(const std::any& any);

    /**
    Emits an error to the observer.

    @return
        True if the error was successfully emitted, false if the producer is already terminated.
    */
    bool EmitOnError(const std::exception_ptr& error);

    /**
    Emits a completion to the observer.

    @return
        True if the completion was successfully emitted, false if the producer is already 
        terminated.
    */
    bool EmitOnCompleted();

    bool IsDisposed() const noexcept override;

protected:
    bool EnsureDisposed() noexcept override final;

    /**
    Override this method in derived classes to do specific disposal work.

    @details
        Any shared pointer should be reset in this method, in order to break potential circular 
        references.
    */
    virtual void OnDispose() noexcept { }

private:
    bool IsTerminated() const noexcept;

    /**
    Marks the producer as terminated.

    @return
        True if the producer was successfully marked as terminated, false if it was already
        terminated.
    */
    bool MarkTerminated() noexcept;

    /**
    Marks the producer as disposed.

    @return
        True if the producer was successfully marked as disposed, false if it was already disposed.

        This method will also mark the producer as terminated.
    */
    bool MarkDisposed() noexcept;

private:
    static constexpr int StateFlagTerminated = 1 << 0;
    static constexpr int StateFlagDisposed = 1 << 1;

private:
    ObserverShim observer_;
    std::atomic<int> state_flags_{};
};

}