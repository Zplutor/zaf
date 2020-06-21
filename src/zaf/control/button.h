#pragma once

#include <zaf/control/clickable_control.h>

namespace zaf {

/**
 Represents a button control.   
 */
class Button : public ClickableControl {
public:
    ZAF_DECLARE_REFLECTION_TYPE();

public:
	Button();
	~Button();

    /**
     Get a value indicating that whether the button is the default button in a dialog.
     */
    bool IsDefault() const;

protected:
    void Initialize() override;
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;

private:
    friend class Dialog;

    void SetIsDefault(bool is_default);
};

}