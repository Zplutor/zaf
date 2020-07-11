#pragma once

#include <zaf/control/textual_control.h>

namespace zaf {

/**
 Represents a label control.	
 */
class Label : public TextualControl {
public:
    ZAF_DECLARE_REFLECTION_TYPE

public:
	Label();
	~Label();

protected:
    void Initialize() override;
};

}