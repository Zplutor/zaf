#pragma once

#include <memory>
#include <zaf/graphic/geometry/geometry.h>
#include <zaf/graphic/geometry/geometry_sink.h>

namespace zaf {

/**
Represents a complex shape that may be composed of arcs, curves, and lines.

You can create PathGeometry instances via GraphicFactory::CreatePathGeometry().
*/
class PathGeometry : public Geometry {
public:
    /**
    Constructs an invalid instance.
    */
    PathGeometry() = default;

    /**
    Constructs an instance with a corresponding COM pointer.
    */
    explicit PathGeometry(COMPtr<ID2D1PathGeometry> inner) : 
        Geometry(inner), 
        inner_(std::move(inner)) { }

    /**
    Gets the number of figures.

    @throw zaf::Error
        Thrown if the operation fails.
    */
    std::size_t GetFigureCount() const;

    /**
    Gets the number of segments.

    @throw zaf::Error
        Thrown if the operation fails.
    */
    std::size_t GetSegmentCount() const;

    /**
    Retrieves the geometry sink that is used to populate the path geometry with figures and 
    segments.

    @throw zaf::Error
        Thrown if the operation fails.
    */
    GeometrySink Open();

    const COMPtr<ID2D1PathGeometry>& Inner() const noexcept {
        return inner_;
    }

private:
    COMPtr<ID2D1PathGeometry> inner_;
};

}