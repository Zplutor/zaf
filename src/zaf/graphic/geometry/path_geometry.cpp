#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/geometry/geometry_sink.h>

namespace zaf {

std::size_t PathGeometry::GetFigureCount() const {

	std::size_t count = 0;
	HRESULT result = handle_->GetFigureCount(&count);

	if (SUCCEEDED(result)) {
		return count;
	}
	else {
		return 0;
	}
}


std::size_t PathGeometry::GetSegmentCount() const {

	std::size_t count = 0;
	HRESULT result = handle_->GetSegmentCount(&count);

	if (SUCCEEDED(result)) {
		return count;
	}
	else {
		return 0;
	}
}


const std::shared_ptr<GeometrySink> PathGeometry::Open() {

	ID2D1GeometrySink* sink_handle = nullptr;
	HRESULT result = handle_->Open(&sink_handle);

	if (SUCCEEDED(result)) {
		return std::make_shared<GeometrySink>(sink_handle);
	}
	else {
		return nullptr;
	}
}

}