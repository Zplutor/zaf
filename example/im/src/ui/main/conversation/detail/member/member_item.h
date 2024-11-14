#pragma once

#include <zaf/control/label.h>
#include <zaf/control/list_control.h>
#include "entity/user.h"

class MemberItem : public zaf::ListItem {
public:
    void Initialize() override;

    void SetMember(const std::shared_ptr<User>& member) {
        gender_ = member->gender;
        label_->SetText(member->name);
    }

protected:
    void Layout(const zaf::Rect&) override;
    void Paint(zaf::Canvas& canvas, const zaf::Rect&) const override;

    void OnMouseEnter(const zaf::MouseEnterInfo& event_info) override {
        NeedRepaint();
    }

    void OnMouseLeave(const zaf::MouseLeaveInfo& event_info) override {
        NeedRepaint();
    }

private:
    std::shared_ptr<zaf::Label> label_;
    User::Gender gender_ = User::Gender::Unknown;
};