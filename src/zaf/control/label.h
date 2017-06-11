#pragma once

#include <zaf/control/textual_control.h>

namespace zaf {

/**
 Represents a label control.	
 */
class Label : public TextualControl {
public:
	Label();
	~Label();

    void Initialize() override;

protected:
    std::wstring GetTypeName() const override;
};

}