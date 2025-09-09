#pragma once

#include <vector>
#include <zaf/control/property_grid/property_list.h>
#include <zaf/dynamic/object_type.h>

namespace zaf::property_grid {

class PropertyTable {
public:
    explicit PropertyTable(std::vector<std::pair<ObjectType*, PropertyList>> inner);

    PropertyList* GetList(ObjectType* type);

    void Clear();

    std::vector<std::pair<ObjectType*, PropertyList>>& Inner() {
        return inner_;
    }

private:
    std::vector<std::pair<ObjectType*, PropertyList>> inner_;
};

}