#include <zaf/control/label.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(Label)
ZAF_DEFINE_TYPE_END


Label::Label() {

}


Label::~Label() {

}


void Label::Initialize() {

    __super::Initialize();

    SetBackgroundColor(Color::Transparent());
}

}