#pragma once

#include <zaf/control/event/sub_menu_popup_event.h>
#include <zaf/control/textual_control.h>

namespace zaf {

class PopupMenu;

class MenuItem : public TextualControl {
public:
    ZAF_DECLARE_TYPE;

public:
    void AddSubMenuItem(const std::shared_ptr<MenuItem>& sub_menu_item);
    void RemoveSubMenuItem(const std::shared_ptr<MenuItem>& sub_menu_item);

    bool HasSubMenuItem() const;

    void PopupSubMenu();
    void CloseSubMenu();

    const std::shared_ptr<PopupMenu>& SubMenu() const {
        return sub_menu_;
    }

    Observable<SubMenuShowInfo> SubMenuShowEvent();
    Observable<SubMenuCloseInfo> SubMenuCloseEvent();

protected:
    void Initialize() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) override;
    zaf::Rect GetTextRect() override;
    zaf::Size CalculatePreferredContentSize(const zaf::Size& bound_size) const override;
    void OnIsSelectedChanged() override;

    virtual zaf::Rect GetSubMenuArrowRect();

private:
    void PaintSubMenuArrow(Canvas& canvas);
    void CheckCreateSubMenu();

private:
    std::shared_ptr<PopupMenu> sub_menu_;
};

}