#pragma once

#include <zaf/control/control.h>
#include <zaf/control/label.h>

class PropertyItem : public zaf::Control {

};


class ValuePropertyItem : public PropertyItem {
public:
    void Initialize() override;

    void SetTitle(const std::wstring& title) {
        title_control_->SetText(title);
    }

    void SetValueControl(const std::shared_ptr<zaf::Control>& control);

protected:
    void Layout(const zaf::Rect&) override;

private:
    std::shared_ptr<zaf::Label> title_control_;
    std::shared_ptr<zaf::Control> value_control_;
};


class GroupPropertyItem : public PropertyItem {
public:
    void Initialize() override;

    void AddItem(const std::shared_ptr<PropertyItem>& item);
};


std::shared_ptr<PropertyItem> CreatePropertyItem(
    const std::wstring& title, 
    const std::shared_ptr<zaf::Control>& value_control);


std::shared_ptr<PropertyItem> CreatePropertyItem(const std::vector<std::shared_ptr<PropertyItem>>& items);