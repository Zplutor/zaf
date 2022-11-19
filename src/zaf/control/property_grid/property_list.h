#pragma once

#include <set>
#include <zaf/object/object_property.h>

namespace zaf::property_grid {

class PropertyList {
public:
    PropertyList(std::vector<ObjectProperty*> inner);

    void EraseByNames(const std::set<std::wstring>& property_names);

    void SortByNames(const std::vector<std::wstring>& property_names);

    std::vector<ObjectProperty*>& Inner() {
        return inner_;
    }

private:
    std::vector<ObjectProperty*> inner_;
};

}