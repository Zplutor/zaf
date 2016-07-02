#pragma once

#include <memory>
#include <zaf/graphic/geometry/geometry.h>

namespace zaf {

class GeometrySink;

class PathGeometry : public Geometry {
public:
	explicit PathGeometry(ID2D1PathGeometry* handle) : 
		Geometry(handle),
		handle_(handle) { 
	
	} 

    std::size_t GetFigureCount(std::error_code& error_code) const;

    std::size_t GetFigureCount() const {
        std::error_code error_code;
        auto result = GetFigureCount(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    std::size_t GetSegmentCount(std::error_code& error_code) const;

    std::size_t GetSegmentCount() const {
        std::error_code error_code;
        auto result = GetSegmentCount(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

	const std::shared_ptr<GeometrySink> Open(std::error_code& error_code);

    const std::shared_ptr<GeometrySink> Open() {
        std::error_code error_code;
        auto result = Open(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

private:
	ID2D1PathGeometry* handle_;
};

}