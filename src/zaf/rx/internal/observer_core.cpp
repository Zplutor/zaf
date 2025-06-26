#include <zaf/rx/internal/observer_core.h>

namespace zaf::rx::internal {
namespace {

class GeneralObserver : public ObserverCore {
public:
    GeneralObserver(
        zaf::OnNext<std::any> on_next,
        zaf::OnError on_error,
        zaf::OnCompleted on_completed)
        :
        on_next_(std::move(on_next)),
        on_error_(std::move(on_error)),
        on_completed_(std::move(on_completed)) {

    }

    void OnNext(const std::any& value) override {
        if (on_next_) {
            on_next_(value);
        }
    }

    void OnError(const std::exception_ptr& error) override {
        if (on_error_) {
            on_error_(error);
        }
    }

    void OnCompleted() override {
        if (on_completed_) {
            on_completed_();
        }
    }

private:
    zaf::OnNext<std::any> on_next_;
    zaf::OnError on_error_;
    zaf::OnCompleted on_completed_;
};

}

std::shared_ptr<ObserverCore> ObserverCore::Create(
    zaf::OnNext<std::any> on_next, 
    zaf::OnError on_error, 
    zaf::OnCompleted on_completed) {

    return std::make_shared<GeneralObserver>(
        std::move(on_next), 
        std::move(on_error),
        std::move(on_completed));
}

}