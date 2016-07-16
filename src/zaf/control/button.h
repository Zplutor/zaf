#pragma once

#include <zaf/control/clickable_control.h>

namespace zaf {

class Button : public ClickableControl {
public:
	Button();
	~Button();

	void Initialize() override;

    bool IsDefault() const;

private:
    friend class Dialog;

    void SetIsDefault(bool is_default);

protected:
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;
};

}