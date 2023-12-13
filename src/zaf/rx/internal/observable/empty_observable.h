#include <zaf/rx/internal/observable/inner_observable.h>

namespace zaf::internal {

class EmptyObservable : public InnerObservable {
public:
    static const std::shared_ptr<EmptyObservable>& Instance();

public:
    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    EmptyObservable() = default;
};

}