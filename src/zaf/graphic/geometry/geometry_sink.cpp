#include <zaf/graphic/geometry/geometry_sink.h>
#include <zaf/graphic/internal/alignment_helper.h>

namespace zaf {

void GeometrySink::BeginFigure(const Point& start_position, BeginFigureOption option) {

    Point aligned_start_position = internal::AlignInRelatedCoordinateSystem(
        start_position,
        0,
        coordinate_origin_,
        aligned_coordinate_origin_);

    GetHandle()->BeginFigure(
        aligned_start_position.ToD2D1POINT2F(),
        static_cast<D2D1_FIGURE_BEGIN>(option));
}


void GeometrySink::AddLine(const Point& end_point) {

    Point aligned_end_point = internal::AlignInRelatedCoordinateSystem(
        end_point, 
        0,
        coordinate_origin_, 
        aligned_coordinate_origin_);

    GetHandle()->AddLine(aligned_end_point.ToD2D1POINT2F());
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

    GetHandle()->AddLines(d2d_points.data(), d2d_points.size());
}


void GeometrySink::AddArc(const ArcSegment& arc_segment) {

    D2D1_ARC_SEGMENT d2d1_segment{};
    d2d1_segment.point = arc_segment.end_point.ToD2D1POINT2F();
    d2d1_segment.size.width = arc_segment.x_radius;
    d2d1_segment.size.height = arc_segment.y_radius;
    d2d1_segment.rotationAngle = arc_segment.rotation_angle;
    d2d1_segment.sweepDirection = static_cast<D2D1_SWEEP_DIRECTION>(arc_segment.sweep_direction);
    d2d1_segment.arcSize = static_cast<D2D1_ARC_SIZE>(arc_segment.arc_size);

    GetHandle()->AddArc(d2d1_segment);
}

}