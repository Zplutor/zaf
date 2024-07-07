#include <zaf/graphic/ellipse.h>
#include <zaf/base/string/to_string.h>

namespace zaf {

ZAF_OBJECT_IMPL(Ellipse);
ZAF_DEFINE_EQUALITY(Ellipse)


std::wstring Ellipse::ToString() const {
    return 
        L'{' + position.ToString() + L"}," + 
        ToWideString(x_radius) + L',' + 
        ToWideString(y_radius);
}


void Ellipse::CloneFrom(const Object& other) {
    *this = As<Ellipse>(other);
}


bool operator==(const Ellipse& ellipse1, const Ellipse& ellipse2) {

    return 
        (ellipse1.position == ellipse2.position) &&
        (ellipse1.x_radius == ellipse2.x_radius) &&
        (ellipse1.y_radius == ellipse2.y_radius);
}


bool operator<(const Ellipse& ellipse1, const Ellipse& ellipse2) {

    if (ellipse1.position < ellipse2.position) {
        return true;
    }

    if (ellipse1.position > ellipse2.position) {
        return false;
    }

    if (ellipse1.x_radius < ellipse2.x_radius) {
        return true;
    }

    if (ellipse1.x_radius > ellipse2.x_radius) {
        return false;
    }

    return ellipse1.y_radius < ellipse2.y_radius;
}

}