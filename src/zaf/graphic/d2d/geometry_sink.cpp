#include <zaf/graphic/d2d/geometry_sink.h>

namespace zaf::d2d {

void GeometrySink::BeginFigure(const Point& start_position, BeginFigureOption option) {

    Ptr()->BeginFigure(
        start_position.ToD2D1POINT2F(),
        static_cast<D2D1_FIGURE_BEGIN>(option));
}


void GeometrySink::AddLine(const Point& end_point) {
    Ptr()->AddLine(end_point.ToD2D1POINT2F());
}


void GeometrySink::AddLines(const std::vector<Point>& points) {

    std::vector<D2D1_POINT_2F> d2d_points;
    d2d_points.reserve(points.size());

    for (const auto& each_point : points) {
        d2d_points.push_back(each_point.ToD2D1POINT2F());
    }

    Ptr()->AddLines(d2d_points.data(), static_cast<UINT32>(d2d_points.size()));
}


void GeometrySink::AddArc(const ArcSegment& arc_segment) {
    Ptr()->AddArc(arc_segment.Inner());
}


void GeometrySink::AddCubicBezier(
    const Point& control_point1,
    const Point& control_point2,
    const Point& end_point) noexcept {

    D2D1_BEZIER_SEGMENT bezier_segment;
    bezier_segment.point1 = control_point1.ToD2D1POINT2F();
    bezier_segment.point2 = control_point2.ToD2D1POINT2F();
    bezier_segment.point3 = end_point.ToD2D1POINT2F();
    Ptr()->AddBezier(&bezier_segment);
}

}