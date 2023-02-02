#pragma once

#include "manager/explore_manager.h"
#include <zaf/creation.h>

class ControlExploreManager : public ExploreManager {
public:
    std::shared_ptr<zaf::Control> GetExploredControl() override {
        if (control_ == nullptr) {
            control_ = CreateExploredControl();
        }
        return control_;
    }

    std::shared_ptr<zaf::Control> GetTargetControl() override {
        return GetExploredControl();
    }

protected:
    virtual std::shared_ptr<zaf::Control> CreateExploredControl() {
        return zaf::Create<zaf::Control>();
    }

private:
    std::shared_ptr<zaf::Control> control_;
};