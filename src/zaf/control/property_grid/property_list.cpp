#include <zaf/control/property_grid/property_list.h>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/erase.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/range.h>

namespace zaf::property_grid {

PropertyList::PropertyList(std::vector<ObjectProperty*> inner) : inner_(std::move(inner)) {

}


void PropertyList::Erase(const std::set<ObjectProperty*>& properties) {

    EraseIf(inner_, [&properties](auto property) {
        return properties.contains(property);
    });
}


void PropertyList::Sort(const std::vector<ObjectProperty*>& priority_properties) {

    std::map<ObjectProperty*, std::size_t> prioritized_orders;
    for (auto index : Range(0, priority_properties.size())) {
        prioritized_orders[priority_properties[index]] = index;
    }

    //Bring prioritized properties to front.
    auto non_prioritized_iterator = std::stable_partition(
        inner_.begin(),
        inner_.end(),
        [&prioritized_orders](auto property) {

        return prioritized_orders.contains(property);
    });

    //Sort prioritized properties.
    std::sort(
        inner_.begin(),
        non_prioritized_iterator,
        [&prioritized_orders](auto property1, auto property2) {

        auto index1 = Find(prioritized_orders, property1);
        auto index2 = Find(prioritized_orders, property2);

        if (index1 && !index2) {
            return true;
        }

        if (!index1 && index2) {
            return false;
        }

        if (index1 && index2) {
            return *index1 < *index2;
        }

        return property1->Name() < property2->Name();
    });
}

}