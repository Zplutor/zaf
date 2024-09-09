#pragma once

#include <memory>
#include <optional>
#include <zaf/graphic/internal/alignment_info.h>
#include <zaf/graphic/d2d/geometry.h>
#include <zaf/graphic/d2d/geometry_sink.h>

namespace zaf::d2d {

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
    Constructs an instance with a corresponding COM pointer and additional alignment info for 
    aligning coordinates to pixels.

    @remark
        This constructor is used by zaf::Canvas internally.
    */
    PathGeometry(COMPtr<ID2D1PathGeometry> inner, const internal::AlignmentInfo& alignment_info) :
        Geometry(inner),
        inner_(std::move(inner)),
        alignment_info_(alignment_info) { }

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
    std::optional<internal::AlignmentInfo> alignment_info_;
};

}