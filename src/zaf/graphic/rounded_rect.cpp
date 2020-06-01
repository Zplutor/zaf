#include <zaf/graphic/rounded_rect.h>
#include <zaf/parsing/parsers/rounded_rect_parser.h>
#include <zaf/reflection/reflection_type_definition.h>

namespace zaf {

ZAF_DEFINE_REFLECTION_TYPE(RoundedRect)
    ZAF_DEFINE_PARSER(RoundedRectParser)
ZAF_DEFINE_END


bool operator==(const RoundedRect& rounded_rect1, const RoundedRect& rounded_rect2) {

    return
        (rounded_rect1.rect == rounded_rect2.rect) &&
        (rounded_rect1.x_radius == rounded_rect2.x_radius) &&
        (rounded_rect1.y_radius == rounded_rect2.y_radius);
}


bool operator<(const RoundedRect& rounded_rect1, const RoundedRect& rounded_rect2) {

    if (rounded_rect1.rect < rounded_rect2.rect) {
        return true;
    }

    if (rounded_rect1.rect > rounded_rect2.rect) {
        return false;
    }

    if (rounded_rect1.x_radius < rounded_rect2.x_radius) {
        return true;
    }

    if (rounded_rect1.x_radius > rounded_rect2.x_radius) {
        return false;
    }

    return rounded_rect1.y_radius < rounded_rect2.y_radius;
}



}