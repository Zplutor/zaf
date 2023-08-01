#include <zaf/control/spin_box.h>
#include <zaf/control/spin_button.h>
#include <zaf/control/spin_edit.h>
#include <zaf/creation.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(SpinBox)
ZAF_DEFINE_TYPE_END;

void SpinBox::Initialize() {

    __super::Initialize();

    edit_ = Create<SpinEdit>();
    up_button_ = Create<SpinButton>();
    down_button_ = Create<SpinButton>();
}


void SpinBox::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);


}

}