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

	std::size_t GetFigureCount() const;
	std::size_t GetSegmentCount() const;

	const std::shared_ptr<GeometrySink> Open();

private:
	ID2D1PathGeometry* handle_;
};

}