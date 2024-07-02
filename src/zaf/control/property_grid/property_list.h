#pragma once

#include <set>
#include <zaf/object/object_property.h>

namespace zaf::property_grid {

class PropertyList {
public:
    PropertyList(std::vector<ObjectProperty*> inner);

    void Erase(const std::set<ObjectProperty*>& properties);

    void Sort(const std::vector<ObjectProperty*>& priority_properties);

    std::vector<ObjectProperty*>& Inner() {
        return inner_;
    }

private:
    std::vector<ObjectProperty*> inner_;
};

}