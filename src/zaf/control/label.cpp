#include <zaf/control/label.h>

namespace zaf {

ZAF_OBJECT_IMPL(Label)


Label::Label() {

}


Label::~Label() {

}


void Label::Initialize() {

    __super::Initialize();

    SetBackgroundColor(Color::Transparent());
}

}