#pragma once 

#include <zaf/control/control.h>
#include "property/property_item.h"

class ExploreManager {
public:
    ExploreManager() { }
    ExploreManager(const ExploreManager&) = delete;
    ExploreManager& operator=(const ExploreManager&) = delete;

    virtual std::shared_ptr<zaf::Control> GetExploredControl() = 0;
    virtual std::vector<std::shared_ptr<PropertyItem>> GetPropertyItems() = 0;
};