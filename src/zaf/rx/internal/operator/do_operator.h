#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

class DoOperator : public ObservableCore {
public:
    DoOperator(
        std::shared_ptr<ObservableCore> source,
        std::shared_ptr<ObserverCore> do_observer);

    std::shared_ptr<Disposable> Subscribe(ObserverShim&& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    std::shared_ptr<ObserverCore> do_observer_;
};

}