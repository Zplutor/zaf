#include <zaf/rx/internal/observable/inner_observable.h>
#include <zaf/base/error/error.h>

namespace zaf::internal {

class ThrowObservable : public InnerObservable {
public:
    explicit ThrowObservable(zaf::Error error);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    zaf::Error error_;
};

}