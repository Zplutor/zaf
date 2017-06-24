#pragma once

#include <zaf/control/clickable_control.h>

namespace zaf {

/**
 Represents a button control.   
 */
class Button : public ClickableControl {
public:
    ZAF_DECLARE_TYPE_NAME();

public:
	Button();
	~Button();

	void Initialize() override;

    /**
     Get a value indicating that whether the button is the default button in a dialog.
     */
    bool IsDefault() const;

private:
    friend class Dialog;

    void SetIsDefault(bool is_default);

protected:
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;
};

}