#pragma once

#include <deque>
#include <variant>
#include <zaf/base/none.h>
#include <zaf/rx/internal/subject/inner_subject.h>

namespace zaf::rx::internal {

class ReplaySubjectCore : public zaf::internal::InnerSubject {
public:
    explicit ReplaySubjectCore(std::optional<std::size_t> replay_size) noexcept;

    std::shared_ptr<zaf::internal::InnerSubscription> Subscribe(
        const std::shared_ptr<ObserverCore>& observer) override;

    void OnNext(const std::any& value) override;
    void OnError(const std::exception_ptr& error) override;
    void OnCompleted() override;

private:
    bool IsTerminated() const noexcept;

private:
    std::deque<std::any> replay_values_;
    std::optional<std::size_t> replay_size_;

    //None for normal termination, std::exception_ptr for error termination.
    std::variant<std::monostate, None, std::exception_ptr> termination_;
};

}