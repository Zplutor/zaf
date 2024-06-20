#pragma once

#include <zaf/control/textual_control.h>

namespace zaf {

/**
 Represents a label control.    
 */
class Label : public TextualControl {
public:
    ZAF_OBJECT;

public:
    Label();
    ~Label();

protected:
    void Initialize() override;
};

ZAF_OBJECT_BEGIN(Label);
ZAF_OBJECT_END;

}