#pragma once

#include <memory>
#include <zaf/graphic/geometry/geometry.h>
#include <zaf/graphic/geometry/geometry_sink.h>

namespace zaf {

/**
 Represents a complex shape that may be composed of arcs, curves, and lines.

 You can create instances via GraphicFactory::CreatePathGeometry.
 */
class PathGeometry : public Geometry {
public:
    PathGeometry() = default;

    /**
     Construct the instance with specified handle.

     See also Geometry::Geometry.
     */
	explicit PathGeometry(ID2D1PathGeometry* handle) : Geometry(handle) { } 

    /**
     Construct the instance with specified handle, as well as an origin of coordinate that used to
     align geometry.
     */
    PathGeometry(
        ID2D1PathGeometry* handle,
        const Point& coordinate_origin, 
        const Point& aligned_coordinate_origin)
        :
        Geometry(handle), 
        coordinate_origin_(coordinate_origin),
        aligned_coordinate_origin_(aligned_coordinate_origin) { }

    /**
     Get the number of figures.

     @param error_code
        An output parameter indicating the error, if any.
     */
    std::size_t GetFigureCount() const;

    /**
     Get the number of segments.

     @param error_code
        An output parameter indicating the error, if any.
     */
    std::size_t GetSegmentCount() const;

    /**
     Retrieve the geometry sink that is used to populate the path geometry with figures and segments.

     @param error_code
        An output parameter indicating the error, if any.
     */
	GeometrySink Open();

private:
	ID2D1PathGeometry* GetActualHandle() const {
        return static_cast<ID2D1PathGeometry*>(GetHandle());
    }

private:
    Point coordinate_origin_;
    Point aligned_coordinate_origin_;
};

}