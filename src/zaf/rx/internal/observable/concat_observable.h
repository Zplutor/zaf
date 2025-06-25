#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::internal {

using ObservableList = std::vector<std::shared_ptr<ObservableCore>>;

class ConcatObservable : public ObservableCore {
public:
    explicit ConcatObservable(ObservableList observables);

    const ObservableList& Observables() const {
        return observables_;
    }

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    ObservableList observables_;
};

}