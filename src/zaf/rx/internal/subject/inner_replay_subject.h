#pragma once

#include <variant>
#include <zaf/base/none.h>
#include <zaf/rx/internal/subject/inner_subject.h>

namespace zaf::internal {

class InnerReplaySubject : public InnerSubject {
public:
    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

    void OnNext(const std::any& value) override;
    void OnError(const std::exception_ptr& error) override;
    void OnCompleted() override;

private:
    /**
    std::any for OnNext(),
    std::exception_ptr for OnError(),
    None for OnCompleted().
    */
    using ReplayItem = std::variant<std::any, std::exception_ptr, None>;

    std::vector<ReplayItem> replay_items_;
};

}