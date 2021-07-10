#include <zaf/graphic/rounded_rect.h>
#include <zaf/object/equality.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(RoundedRect)
ZAF_DEFINE_PROPERTY_WITH_FIELD(XRadius, x_radius)
ZAF_DEFINE_PROPERTY_WITH_FIELD(YRadius, y_radius)
ZAF_DEFINE_PROPERTY_WITH_FIELD(Rect, rect)
ZAF_DEFINE_TYPE_END

ZAF_DEFINE_EQUALITY(RoundedRect)


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