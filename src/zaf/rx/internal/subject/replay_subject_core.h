#pragma once

#include <deque>
#include <mutex>
#include <zaf/rx/internal/subject/subject_core.h>

namespace zaf::rx::internal {

class ReplaySubjectCore : public SubjectCore {
public:
    explicit ReplaySubjectCore(std::optional<std::size_t> replay_size) noexcept;

    std::shared_ptr<Disposable> Subscribe(ObserverShim&& observer) override;

    void OnNext(const std::any& value) override;

private:
    const std::optional<std::size_t> replay_size_;
    std::deque<std::any> replay_values_;
    std::mutex lock_;
};

}