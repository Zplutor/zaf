#pragma once

#include <zaf/control/label.h>

class OperateItem : public zaf::Control {
public:
    void Initialize() override;

    void SetTitle(const std::wstring& name, std::size_t ident_level);

    void SetTitle(const std::wstring& name) {
        SetTitle(name, 0);
    }

    void SetValueControl(const std::shared_ptr<zaf::Control>& control) {
        value_control_ = control;
        AddChild(value_control_);
    }

    void SetItemHeight(float height) {
        height_ = height;
        SetHeight(height_);
    }

    float GetItemHeight() const {
        return height_;
    }

protected:
    void Layout(const zaf::Rect&) override;

private:
    std::shared_ptr<zaf::Label> name_label_;
    std::shared_ptr<zaf::Control> value_control_;
    float height_ = 0;
};


class OperateItemGroup : public zaf::Control {
public:
    void AddOperateItem(const std::shared_ptr<OperateItem>& item) {
        total_height_ += item->GetHeight();
        AddChild(item);
    }

private:
    float total_height_;
};