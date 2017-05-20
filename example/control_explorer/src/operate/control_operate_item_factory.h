#pragma once

#include "operate/operate_item_factory.h"

class ControlOperateItemFactory : public OperateItemFactory {
protected:
    void AddOperateItems(std::vector<std::shared_ptr<zaf::Control>>& items) override;

private:
    void AddOptionItems(std::vector<std::shared_ptr<zaf::Control>>& items);
    void AddSizeItems(std::vector<std::shared_ptr<zaf::Control>>& items);
    std::shared_ptr<zaf::Control> CreateBorderItems();
    std::shared_ptr<zaf::Control> CreatePaddingItems();
};