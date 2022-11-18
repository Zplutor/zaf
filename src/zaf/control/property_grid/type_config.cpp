#include <zaf/control/property_grid/type_config.h>
#include <map>
#include <string_view>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/container/utility/range.h>
#include <zaf/base/container/utility/sort.h>
#include <zaf/control/property_grid/enum_value_view.h>
#include <zaf/control/property_grid/text_value_view.h>
#include <zaf/creation.h>

namespace zaf::property_grid {

std::shared_ptr<ValueView> TypeConfig::CreateValueView() {

    return Create<TextValueView>();
}


void TypeConfig::ArrangeProperties(
    std::vector<ObjectProperty*>& properties,
    const std::vector<std::wstring>& prioritized_properties) {

    std::map<std::wstring_view, std::size_t> prioritized_orders;
    for (auto index : zaf::Range(0, prioritized_properties.size())) {
        prioritized_orders[prioritized_properties[index]] = index;
    }

    //Bring prioritized properties to front.
    auto non_prioritized_iterator = std::stable_partition(
        properties.begin(), 
        properties.end(), 
        [&prioritized_orders](auto property) {

        return Contain(prioritized_orders, property->GetName());
    });

    //Sort prioritized properties.
    std::sort(
        properties.begin(),
        non_prioritized_iterator,
        [&prioritized_orders](auto property1, auto property2) {
    
        auto index1 = Find(prioritized_orders, property1->GetName());
        auto index2 = Find(prioritized_orders, property2->GetName());

        if (index1 && !index2) {
            return true;
        }

        if (!index1 && index2) {
            return false;
        }

        if (index1 && index2) {
            return *index1 < *index2;
        }

        return property1->GetName() < property2->GetName();
    });
}

}
