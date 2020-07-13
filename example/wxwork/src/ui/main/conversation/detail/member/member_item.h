#pragma once

#include <zaf/control/list_control.h>
#include "entity/user.h"

class MemberItem : public zaf::ListItem {
public:
    void Initialize() override;

    void SetMember(const std::shared_ptr<User>& member) {
        gender_ = member->gender;
        SetText(member->name);
    }

protected:
    void Paint(zaf::Canvas& canvas, const zaf::Rect&) override;;

    zaf::Rect GetTextRect() override;

    void MouseEnter(const std::shared_ptr<zaf::Control>& control) override {
        NeedRepaint();
    }

    void MouseLeave(const std::shared_ptr<zaf::Control>& control) override {
        NeedRepaint();
    }

private:
    User::Gender gender_ = User::Gender::Unknown;
};