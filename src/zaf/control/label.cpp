#include <zaf/control/label.h>

namespace zaf {

Label::Label() {

}


Label::~Label() {

}


void Label::Initialize() {

    __super::Initialize();

    SetBackgroundColorPicker([](const Control&) {
        return Color::Transparent;
    });
}

}