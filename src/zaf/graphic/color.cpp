#include <zaf/graphic/color.h>
#include <zaf/parsing/parsers/color_parser.h>
#include <zaf/reflection/reflection_type_definition.h>

namespace zaf {

ZAF_DEFINE_REFLECTION_TYPE(Color, ReflectionObject);

const Color Color::Black = Color(0, 0, 0);
const Color Color::Blue = Color(0, 0, 1);
const Color Color::Cyan = Color(0, 1, 1);
const Color Color::Gray = Color(0.5, 0.5, 0.5);
const Color Color::Green = Color(0, 0.5, 0);
const Color Color::Lime = Color(0, 1, 0);
const Color Color::Magenta = Color(1, 0, 1);
const Color Color::Red = Color(1, 0, 0);
const Color Color::Transparent = Color(0, 0, 0, 0);
const Color Color::White = Color(1, 1, 1);
const Color Color::Yellow = Color(1, 1, 0);

}