#include <zaf/control/internal/triangle_geometry.h>
#include <zaf/application.h>
#include <zaf/graphic/resource_factory.h>

namespace zaf::internal {

Geometry CreateTriangleGeometry(
    float canvas_width,
    float canvas_height,
    float bottom_edge_length,
    float rotate_angle) {

    //Firstly, create a geometry that describes an triangle which direction is up,
    //and then rotate the geometry according to the actual direction.

    float height = bottom_edge_length / 2;
    float half_height = height / 2;

    Point center_point(canvas_width / 2, canvas_height / 2);

    Point top_point(center_point.x, center_point.y - half_height);
    Point left_point(center_point.x - height, center_point.y + half_height);
    Point right_point(center_point.x + height, center_point.y + half_height);

    auto triangle_geometry = GetGraphicFactory().CreatePathGeometry();
    auto triangle_geometry_sink = triangle_geometry.Open();
    triangle_geometry_sink.BeginFigure(top_point, GeometrySink::BeginFigureOption::Fill);
    triangle_geometry_sink.AddLine(left_point);
    triangle_geometry_sink.AddLine(right_point);
    triangle_geometry_sink.EndFigure(GeometrySink::EndFigureOption::Close);
    triangle_geometry_sink.Close();

    return GetGraphicFactory().CreateTransformedGeometry(
        triangle_geometry,
        TransformMatrix::Rotation(rotate_angle, center_point));
}

}