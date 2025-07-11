#pragma once

namespace zaf {

class BeginRunInfo {

};


class EndRunInfo {

};


class SessionEndInfo {

};


class UnhandledExceptionInfo {
public:
    explicit UnhandledExceptionInfo(std::exception_ptr exception) noexcept : 
        exception_(std::move(exception)) {

    }

    const std::exception_ptr& Exception() const noexcept {
        return exception_;
    }

    bool IsHandled() const noexcept {
        return is_handled_;
    }

    void MarkAsHandled() noexcept {
        is_handled_ = true;
    }

private:
    std::exception_ptr exception_;
    bool is_handled_{};
};

}