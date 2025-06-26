#pragma once

#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/internal/operator/mapper.h>

namespace zaf::rx::internal {

class MapOperator : public ObservableCore {
public:
    MapOperator(
        std::shared_ptr<ObservableCore> source,
        Mapper mapper);

    std::shared_ptr<SubscriptionCore> Subscribe(
        const std::shared_ptr<ObserverCore>& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    Mapper mapper_;
};

}