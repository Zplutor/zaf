#include <zaf/control/label.h>
#include <zaf/parsing/parsers/label_parser.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(Label)
    ZAF_DEFINE_TYPE_PARSER(LabelParser)
ZAF_DEFINE_TYPE_END


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