#pragma once

#include <zaf/control/textual_control.h>

namespace zaf {

class PopupMenu;

class MenuItem : public TextualControl {
public:
    void AddSubMenuItem(const std::shared_ptr<MenuItem>& sub_menu_item);
    void RemoveSubMenuItem(const std::shared_ptr<MenuItem>& sub_menu_item);

    bool HasSubMenuItem() const;

protected:
    void Initialize() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) override;
    zaf::Rect GetTextRect() override;
    zaf::Size CalculatePreferredContentSize(const zaf::Size& bound_size) const override;

private:
    zaf::Rect GetSubMenuArrowRect() const;
    void AdjustAppearence();
    void PopupSubMenu();

private:
    std::shared_ptr<PopupMenu> sub_menu_;
};

}