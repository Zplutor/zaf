#pragma once

#include <zaf/control/clickable_control.h>

namespace zaf {

/**
 Represents a button control.   
 */
class Button : public ClickableControl {
public:
    ZAF_OBJECT;

public:
    Button();
    ~Button();

    /**
     Get a value indicating that whether the button is the default button in a dialog.
     */
    bool IsDefault() const;

protected:
    void Initialize() override;
    void UpdateStyle() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const override;

private:
    friend class Dialog;

    void SetIsDefault(bool is_default);

private:
    bool is_default_{ false };
};

ZAF_OBJECT_BEGIN(Button);
ZAF_OBJECT_END;

}