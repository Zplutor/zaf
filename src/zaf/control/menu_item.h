#pragma once

#include <zaf/control/event/sub_menu_popup_event.h>
#include <zaf/control/textual_control.h>

namespace zaf {

class PopupMenu;

class MenuItem : public TextualControl {
public:
    ZAF_OBJECT;

public:
    void AddSubMenuItem(const std::shared_ptr<MenuItem>& sub_menu_item);
    void RemoveSubMenuItem(const std::shared_ptr<MenuItem>& sub_menu_item);

    bool HasSubMenuItem() const;

    void PopupSubMenu();
    void CloseSubMenu();

    const std::shared_ptr<PopupMenu>& SubMenu() const {
        return sub_menu_;
    }

    rx::Observable<SubMenuShowInfo> SubMenuShowEvent() const;
    rx::Observable<SubMenuCloseInfo> SubMenuCloseEvent() const;

protected:
    void Initialize() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const override;
    zaf::Rect DetermineTextRect() override;
    zaf::Size CalculatePreferredContentSize(const zaf::Size& bound_size) const override;
    void OnIsSelectedChanged() override;

    virtual zaf::Rect GetSubMenuArrowRect() const;

private:
    void PaintSubMenuArrow(Canvas& canvas) const;
    void CheckCreateSubMenu();

private:
    std::shared_ptr<PopupMenu> sub_menu_;

    Event<SubMenuShowInfo> sub_menu_show_event_;
    Event<SubMenuCloseInfo> sub_menu_close_event_;
};

ZAF_OBJECT_BEGIN(MenuItem);
ZAF_OBJECT_END;

}