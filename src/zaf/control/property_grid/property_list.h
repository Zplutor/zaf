#pragma once

#include <set>
#include <zaf/dynamic/object_property.h>

namespace zaf::property_grid {

class PropertyList {
public:
    PropertyList(std::vector<dynamic::ObjectProperty*> inner);

    void Erase(const std::set<dynamic::ObjectProperty*>& properties);

    void Sort(const std::vector<dynamic::ObjectProperty*>& priority_properties);

    std::vector<dynamic::ObjectProperty*>& Inner() {
        return inner_;
    }

private:
    std::vector<dynamic::ObjectProperty*> inner_;
};

}