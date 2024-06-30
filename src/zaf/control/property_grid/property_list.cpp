#include <zaf/control/property_grid/property_list.h>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/erase.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/range.h>

namespace zaf::property_grid {

PropertyList::PropertyList(std::vector<ObjectProperty*> inner) : inner_(std::move(inner)) {

}


void PropertyList::EraseByNames(const std::set<std::wstring>& property_names) {

    EraseIf(inner_, [&property_names](auto property) {
        return Contain(property_names, property->Name());
    });
}


void PropertyList::SortByNames(const std::vector<std::wstring>& property_names) {

    std::map<std::wstring_view, std::size_t> prioritized_orders;
    for (auto index : Range(0, property_names.size())) {
        prioritized_orders[property_names[index]] = index;
    }

    //Bring prioritized properties to front.
    auto non_prioritized_iterator = std::stable_partition(
        inner_.begin(),
        inner_.end(),
        [&prioritized_orders](auto property) {

        return zaf::Contain(prioritized_orders, property->Name());
    });

    //Sort prioritized properties.
    std::sort(
        inner_.begin(),
        non_prioritized_iterator,
        [&prioritized_orders](auto property1, auto property2) {

        auto index1 = Find(prioritized_orders, property1->Name());
        auto index2 = Find(prioritized_orders, property2->Name());

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