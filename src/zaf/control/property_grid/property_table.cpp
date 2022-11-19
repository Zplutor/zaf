#include <zaf/control/property_grid/property_table.h>

namespace zaf::property_grid {

PropertyTable::PropertyTable(std::vector<std::pair<ObjectType*, PropertyList>> inner) :
    inner_(std::move(inner)) {

}


PropertyList* PropertyTable::GetList(ObjectType* type) {

    for (auto& each_pair : inner_) {

        if (each_pair.first == type) {
            return &each_pair.second;
        }
    }

    return nullptr;
}


void PropertyTable::Clear() {
    inner_.clear();
}

}