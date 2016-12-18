#pragma once 

#include <zaf/control/control.h>

class ControlExploreManager {
public:
    virtual ~ControlExploreManager() { }

    void Initialize();

    const std::shared_ptr<zaf::Control>& GetControl() const {
        return control_;
    }

    const std::vector<std::shared_ptr<zaf::Control>>& GetOperatePanels() const {
        return operate_panels_;
    }

protected:
    virtual void Initialize(
        std::shared_ptr<zaf::Control>& control,
        std::vector<std::shared_ptr<zaf::Control>>& operate_panels) = 0;

private:
    std::shared_ptr<zaf::Control> control_;
    std::vector<std::shared_ptr<zaf::Control>> operate_panels_;
};