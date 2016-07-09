#pragma once

#include <zaf/control/textual_control.h>

namespace zaf {

/**
 Represents a label control that displays static text.	
 */
class Label : public TextualControl {
public:
	/**
	 Initialize the instance.
	 */
	Label();

	/**
	 Destroy the instance.
	 */
	~Label();

    void Initialize() override;
};

}