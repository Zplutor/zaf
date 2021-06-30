#include <zaf/graphic/color.h>
#include <zaf/parsing/parsers/color_parser.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_EQUALITY(Color)

ZAF_DEFINE_TYPE(Color)
    ZAF_DEFINE_PARSER(ColorParser)
ZAF_DEFINE_END

const Color& Color::Black() {
    static Color black{ 0, 0, 0 };
    return black;
}

const Color& Color::Blue() {
    static Color blue{ 0, 0, 1 };
    return blue;
}

const Color& Color::Cyan() {
    static Color cyan{ 0, 1, 1 };
    return cyan;
}

const Color& Color::Gray() {
    static Color gray{ 0.5, 0.5, 0.5 };
    return gray;
}

const Color& Color::Green() {
    static Color green{ 0, 0.5, 0 };
    return green;
}

const Color& Color::Lime() {
    static Color lime{ 0, 1, 0 };
    return lime;
}

const Color& Color::Magenta() {
    static Color megenta{ 1, 0, 1 };
    return megenta;
}

const Color& Color::Red() {
    static Color red{ 1, 0, 0 };
    return red;
}

const Color& Color::Transparent() {
    static Color transparent{ 0, 0, 0, 0 };
    return transparent;
}

const Color& Color::White() {
    static Color white{ 1, 1, 1 };
    return white;
}

const Color& Color::Yellow() {
    static Color yellow{ 1, 1, 0 };
    return yellow;
}

}