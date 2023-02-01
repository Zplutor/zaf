#pragma once 

#include <zaf/control/control.h>

class ExploreManager {
public:
    ExploreManager() { }
    ExploreManager(const ExploreManager&) = delete;
    ExploreManager& operator=(const ExploreManager&) = delete;

    virtual std::shared_ptr<zaf::Control> GetExploredControl() = 0;
};