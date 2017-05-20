#pragma once

#include <zaf/control/list_box.h>
#include "operate/control_operate_item_factory.h"

class ListBoxOperateItemFactory : public ControlOperateItemFactory {
public:
    void AddOperateItems(std::vector<std::shared_ptr<zaf::Control>>& items) override;

private:
    std::shared_ptr<zaf::ListBox> GetListBox() const {
        return std::dynamic_pointer_cast<zaf::ListBox>(GetTargetControl());
    }

    std::shared_ptr<zaf::Control> CreateItemHeightOperateItem();
};