#include <zaf/graphic/geometry/geometry_sink.h>
#include <zaf/graphic/internal/alignment_helper.h>

namespace zaf {

void GeometrySink::BeginFigure(const Point& start_position, BeginFigureOption option) {

    Ptr()->BeginFigure(
        ToAlignedD2DPoint(start_position),
        static_cast<D2D1_FIGURE_BEGIN>(option));
}


void GeometrySink::AddLine(const Point& end_point) {
    Ptr()->AddLine(ToAlignedD2DPoint(end_point));
}


void GeometrySink::AddLines(const std::vector<Point>& points) {

    std::vector<D2D1_POINT_2F> d2d_points;
    d2d_points.reserve(points.size());

    for (const auto& each_point : points) {
        d2d_points.push_back(ToAlignedD2DPoint(each_point));
    }

    Ptr()->AddLines(d2d_points.data(), static_cast<UINT32>(d2d_points.size()));
}


void GeometrySink::AddArc(const ArcSegment& arc_segment) {
    Ptr()->AddArc(arc_segment.Inner());
}


D2D1_POINT_2F GeometrySink::ToAlignedD2DPoint(const Point& point) const {

    D2D1_POINT_2F d2d_position{};
    if (!alignment_info_) {
        return point.ToD2D1POINT2F();
    }

    auto aligned_point = internal::AlignInRelatedCoordinateSystem(
        point,
        0,
        alignment_info_->dpi,
        alignment_info_->coordinate_origin,
        alignment_info_->aligned_coordinate_origin);

    return aligned_point.ToD2D1POINT2F();
}

}