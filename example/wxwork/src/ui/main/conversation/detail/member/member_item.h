#pragma once

#include <zaf/control/list_control.h>
#include "entity/user.h"

class MemberItem : public zaf::ListControl::Item {
public:
    void Initialize() override;

    void SetMember(const std::shared_ptr<User>& member) {
        gender_ = member->gender;
        SetText(member->name);
    }

protected:
    void Paint(zaf::Canvas& canvas, const zaf::Rect&) override;;

    zaf::Rect GetTextRect() override;

    void MouseEnter() override {
        NeedRepaint();
    }

    void MouseLeave() override {
        NeedRepaint();
    }

private:
    User::Gender gender_ = User::Gender::Unknown;
};