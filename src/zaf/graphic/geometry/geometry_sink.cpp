#include <zaf/graphic/geometry/geometry_sink.h>
#include <zaf/graphic/internal/alignment_helper.h>

namespace zaf {

void GeometrySink::BeginFigure(const Point& start_position, BeginFigureOption option) {

    Point aligned_start_position = internal::AlignInRelatedCoordinateSystem(
        start_position,
        0,
        coordinate_origin_,
        aligned_coordinate_origin_);

    Inner()->BeginFigure(
        aligned_start_position.ToD2D1POINT2F(),
        static_cast<D2D1_FIGURE_BEGIN>(option));
}


void GeometrySink::AddLine(const Point& end_point) {

    Point aligned_end_point = internal::AlignInRelatedCoordinateSystem(
        end_point, 
        0,
        coordinate_origin_, 
        aligned_coordinate_origin_);

    Inner()->AddLine(aligned_end_point.ToD2D1POINT2F());
}


void GeometrySink::AddLines(const std::vector<Point>& points) {

    std::vector<D2D1_POINT_2F> d2d_points;
    d2d_points.reserve(points.size());

    for (const auto& each_point : points) {

        Point aligned_point = internal::AlignInRelatedCoordinateSystem(
            each_point, 
            0,
            coordinate_origin_, 
            aligned_coordinate_origin_);

        d2d_points.push_back(aligned_point.ToD2D1POINT2F());
    }

    Inner()->AddLines(d2d_points.data(), static_cast<UINT32>(d2d_points.size()));
}


void GeometrySink::AddArc(const ArcSegment& arc_segment) {
    Inner()->AddArc(arc_segment.Inner());
}

}