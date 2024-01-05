#include <zaf/graphic/geometry/geometry_sink.h>

namespace zaf {

void GeometrySink::BeginFigure(const Point& start_position, BeginFigureOption option) {
    Inner()->BeginFigure(start_position.ToD2D1POINT2F(), static_cast<D2D1_FIGURE_BEGIN>(option));
}


void GeometrySink::AddLine(const Point& end_point) {
    Inner()->AddLine(end_point.ToD2D1POINT2F());
}


void GeometrySink::AddLines(const std::vector<Point>& points) {

    std::vector<D2D1_POINT_2F> d2d_points;
    d2d_points.reserve(points.size());

    for (const auto& each_point : points) {
        d2d_points.push_back(each_point.ToD2D1POINT2F());
    }

    Inner()->AddLines(d2d_points.data(), static_cast<UINT32>(d2d_points.size()));
}


void GeometrySink::AddArc(const ArcSegment& arc_segment) {
    Inner()->AddArc(arc_segment.Inner());
}

}