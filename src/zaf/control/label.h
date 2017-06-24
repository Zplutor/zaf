#pragma once

#include <zaf/control/textual_control.h>

namespace zaf {

/**
 Represents a label control.	
 */
class Label : public TextualControl {
public:
    ZAF_DECLARE_TYPE_NAME();

	Label();
	~Label();

    void Initialize() override;
};

}