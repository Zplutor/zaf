#pragma once

#include <zaf/control/control.h>

namespace zaf {

class VerticalBox : public Control {
public:
    ZAF_DECLARE_REFLECTION_TYPE

protected:
    void Initialize() override;
};


class HorizontalBox : public Control {
public:
    ZAF_DECLARE_REFLECTION_TYPE

protected:
    void Initialize() override;
};

}