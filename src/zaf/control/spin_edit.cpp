#include <zaf/control/spin_edit.h>

namespace zaf {

ZAF_OBJECT_IMPL(SpinEdit);

void SpinEdit::Initialize() {

    __super::Initialize();

    SetPadding(zaf::Frame{ 4, 0, 0, 0 });
    SetBorder({});
    SetParagraphAlignment(dwrite::ParagraphAlignment::Center);
    SetTextValidator(GetNumberTextValidator());
}

}