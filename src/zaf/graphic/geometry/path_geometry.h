#pragma once

#include <memory>
#include <zaf/graphic/geometry/geometry.h>
#include <zaf/graphic/geometry/geometry_sink.h>

namespace zaf {

/**
 Represents a complex shape that may be composed of arcs, curves, and lines.

 You can create instances via ResourceFactory::CreatePathGeometry.
 */
class PathGeometry : public Geometry {
public:
    PathGeometry() { }

    /**
     Construct the instance with specified handle.

     See also Geometry::Geometry.
     */
	explicit PathGeometry(ID2D1PathGeometry* handle) : Geometry(handle) { } 

    /**
     Get the number of figures.

     @param error_code
        An output parameter indicating the error, if any.

     @return
        Return 0 if error occurs.
     */
    std::size_t GetFigureCount(std::error_code& error_code) const;

    std::size_t GetFigureCount() const {
        std::error_code error_code;
        auto result = GetFigureCount(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Get the number of segments.

     @param error_code
        An output parameter indicating the error, if any.

     @return
        Return 0 if error occurs.
     */
    std::size_t GetSegmentCount(std::error_code& error_code) const;

    std::size_t GetSegmentCount() const {
        std::error_code error_code;
        auto result = GetSegmentCount(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Retrieve the geometry sink that is used to populate the path geometry with figures and segments.

     @param error_code
        An output parameter indicating the error, if any.

     @return
        Return nullptr if error occurs.
     */
	const GeometrySink Open(std::error_code& error_code);

    const GeometrySink Open() {
        std::error_code error_code;
        auto result = Open(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

private:
	ID2D1PathGeometry* GetActualHandle() const {
        return dynamic_cast<ID2D1PathGeometry*>(GetHandle());
    }
};

}