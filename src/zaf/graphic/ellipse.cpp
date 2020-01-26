#include <zaf/graphic/ellipse.h>
#include <zaf/parsing/parsers/ellipse_parser.h>
#include <zaf/reflection/reflection_type_definition.h>

namespace zaf {

ZAF_DEFINE_REFLECTION_TYPE(Ellipse, ReflectionObject);


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