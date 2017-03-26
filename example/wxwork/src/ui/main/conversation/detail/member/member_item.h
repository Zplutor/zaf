#pragma once

#include <zaf/control/list_control.h>
#include "entity/user.h"

class MemberItem : public zaf::ListControl::Item {
public:
    void Initialize() override;

    void SetMember(const std::shared_ptr<User>& member) {
        SetText(member->name);
    }

protected:
    void MouseEnter() override {
        NeedRepaint();
    }

    void MouseLeave() override {
        NeedRepaint();
    }

private:
    
};