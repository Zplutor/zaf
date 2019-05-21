#include <zaf/graphic/geometry/geometry_sink.h>
#include <zaf/graphic/internal/alignment_helper.h>

namespace zaf {

void GeometrySink::BeginFigure(const Point& start_position, BeginFigureOption option) {

    Point aligned_start_position = internal::AlignWithCoordinateOrigin(
        start_position,
        coordinate_origin_);

    GetHandle()->BeginFigure(
        aligned_start_position.ToD2D1POINT2F(),
        static_cast<D2D1_FIGURE_BEGIN>(option));
}


void GeometrySink::AddLine(const Point& end_point) {

    Point aligned_end_point = internal::AlignWithCoordinateOrigin(end_point, coordinate_origin_);
    GetHandle()->AddLine(aligned_end_point.ToD2D1POINT2F());
}


void GeometrySink::AddLines(const std::vector<Point>& points) {

	std::vector<D2D1_POINT_2F> d2d_points;
	d2d_points.reserve(points.size());

	for (const auto& each_point : points) {

        Point aligned_point = internal::AlignWithCoordinateOrigin(each_point, coordinate_origin_);
		d2d_points.push_back(aligned_point.ToD2D1POINT2F());
	}

	GetHandle()->AddLines(d2d_points.data(), d2d_points.size());
}

}