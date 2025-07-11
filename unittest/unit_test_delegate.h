#pragma once

#include <zaf/base/as.h>
#include <zaf/application.h>
#include <zaf/application_delegate.h>

namespace zaf::testing {

class UnitTestDelegate : public ApplicationDelegate {
public:
    using UnhandledExceptionHandler = std::function<void(UnhandledExceptionInfo&)>;

    class UnhandledExceptionHandlerGuard : zaf::NonCopyable {
    public:
        UnhandledExceptionHandlerGuard() = default;
        UnhandledExceptionHandlerGuard(UnhandledExceptionHandlerGuard&& other) {
            should_unregister_ = other.should_unregister_;
            other.should_unregister_ = false;
        }

        ~UnhandledExceptionHandlerGuard();

    private:
        bool should_unregister_{ true };
    };

public:
    static UnitTestDelegate& Instance() {
        return *zaf::As<UnitTestDelegate>(zaf::Application::Instance().Delegate());
    }

public:
    [[nodiscard]]
    UnhandledExceptionHandlerGuard RegisterUnhandledExceptionHandler(
        UnhandledExceptionHandler handler) {
        unhandled_exception_handler_ = std::move(handler);
        return UnhandledExceptionHandlerGuard{};
    }

    std::exception_ptr LastUnhandledException() const noexcept {
        return last_unhandled_exception_;
    }

private:
    void OnUnhandledException(UnhandledExceptionInfo& event_info) override {

        if (unhandled_exception_handler_) {
            unhandled_exception_handler_(event_info);
        }
        else {
            last_unhandled_exception_ = event_info.Exception();
            event_info.MarkAsHandled();
        }
    }

private:
    std::exception_ptr last_unhandled_exception_;
    UnhandledExceptionHandler unhandled_exception_handler_;
};


inline UnitTestDelegate::UnhandledExceptionHandlerGuard::~UnhandledExceptionHandlerGuard() {
    if (should_unregister_) {
        UnitTestDelegate::Instance().unhandled_exception_handler_ = nullptr;
    }
}

}