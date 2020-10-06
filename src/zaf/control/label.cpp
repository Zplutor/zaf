#include <zaf/control/label.h>
#include <zaf/parsing/parsers/label_parser.h>
#include <zaf/reflection/reflection_type_definition.h>

namespace zaf {

ZAF_DEFINE_REFLECTION_TYPE(Label)
    ZAF_DEFINE_PARSER(LabelParser)
ZAF_DEFINE_END


Label::Label() {

}


Label::~Label() {

}


void Label::Initialize() {

    __super::Initialize();

    SetBackgroundColorPicker([](const Control&) {
        return Color::Transparent();
    });
}

}