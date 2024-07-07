#include <zaf/graphic/rounded_rect.h>
#include <zaf/base/string/to_string.h>
#include <zaf/object/equality.h>

namespace zaf {

ZAF_OBJECT_IMPL(RoundedRect);
ZAF_DEFINE_EQUALITY(RoundedRect)


std::wstring RoundedRect::ToString() const {
    return L'{' + rect.ToString() + L"}," + ToWideString(x_radius) + L',' + ToWideString(y_radius);
}


void RoundedRect::CloneFrom(const Object& other) {
    *this = As<RoundedRect>(other);
}


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