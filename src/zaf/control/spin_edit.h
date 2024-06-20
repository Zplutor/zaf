#pragma once

#include <zaf/control/rich_edit.h>

namespace zaf {

class SpinEdit : public RichEdit {
public:
    ZAF_OBJECT;

protected:
    void Initialize() override;
};

ZAF_OBJECT_BEGIN(SpinEdit);
ZAF_OBJECT_END;

}