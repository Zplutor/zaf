#pragma once

#include <zaf/control/control.h>

class OperateItemFactory {
public:
    virtual ~OperateItemFactory() { }

    const std::shared_ptr<zaf::Control>& GetTargetControl() const {
        return target_control_;
    }

    void SetTargetControl(const std::shared_ptr<zaf::Control>& control) {
        target_control_ = control;
    }

    std::vector<std::shared_ptr<zaf::Control>> CreateOperateItems() {
        std::vector<std::shared_ptr<zaf::Control>> items;
        AddOperateItems(items);
        return items;
    }

protected:
    virtual void AddOperateItems(std::vector<std::shared_ptr<zaf::Control>>& item) = 0;

private:
    std::shared_ptr<zaf::Control> target_control_;
};