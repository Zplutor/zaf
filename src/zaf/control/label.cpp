#include <zaf/control/label.h>
#include <zaf/serialization/types.h>

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


std::wstring Label::GetTypeName() const {
    return type::Label;
}

}