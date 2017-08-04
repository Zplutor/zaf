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

    std::vector<std::shared_ptr<PropertyItem>> GetPropertyItems() override {
        if (property_items_.empty()) {
            CreatePropertyItems(property_items_);
        }
        return property_items_;
    }

protected:
    virtual std::shared_ptr<zaf::Control> CreateExploredControl() {
        return zaf::Create<zaf::Control>();
    }

    virtual void CreatePropertyItems(std::vector<std::shared_ptr<PropertyItem>>& items);

    virtual std::shared_ptr<zaf::Control> GetTargetControl() {
        return GetExploredControl();
    }

private:
    void AddOptionItems(std::vector<std::shared_ptr<PropertyItem>>& items);
    void AddSizeItems(std::vector<std::shared_ptr<PropertyItem>>& items);
    void AddBorderItems(std::vector<std::shared_ptr<PropertyItem>>& items);
    void AddPaddingItems(std::vector<std::shared_ptr<PropertyItem>>& items);

private:
    std::shared_ptr<zaf::Control> control_;
    std::vector<std::shared_ptr<PropertyItem>> property_items_;
};