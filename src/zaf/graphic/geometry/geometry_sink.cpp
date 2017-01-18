#include <zaf/graphic/geometry/geometry_sink.h>

namespace zaf {

void GeometrySink::AddLines(const std::vector<Point>& points) {

	std::vector<D2D1_POINT_2F> d2d_points;
	d2d_points.reserve(points.size());

	for (const auto& each_point : points) {
		d2d_points.push_back(each_point.ToD2D1POINT2F());
	}

	GetHandle()->AddLines(d2d_points.data(), d2d_points.size());
}

}