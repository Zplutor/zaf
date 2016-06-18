#pragma once

#include <zaf/control/clickable_control.h>

namespace zaf {

class Button : public ClickableControl {
public:
	Button();
	~Button();

	void Initialize() override;

    bool IsDefault() const;
    void SetIsDefault(bool is_default);
};

}