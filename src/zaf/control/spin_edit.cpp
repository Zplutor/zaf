#include <zaf/control/spin_edit.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(SpinEdit)
ZAF_DEFINE_TYPE_END;

void SpinEdit::Initialize() {

    __super::Initialize();

    SetPadding(zaf::Frame{ 4, 0, 0, 0 });
    SetBorder({});
    SetParagraphAlignment(ParagraphAlignment::Center);
    SetTextValidator(GetNumberTextValidator());
}

}