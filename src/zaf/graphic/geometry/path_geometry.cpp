#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/geometry/geometry_sink.h>

namespace zaf {

std::size_t PathGeometry::GetFigureCount(std::error_code& error_code) const {

	std::size_t count = 0;
	HRESULT result = GetActualHandle()->GetFigureCount(&count);

    error_code = MakeComErrorCode(result);
	if (IsSucceeded(error_code)) {
		return count;
	}
	else {
		return 0;
	}
}


std::size_t PathGeometry::GetSegmentCount(std::error_code& error_code) const {

	std::size_t count = 0;
	HRESULT result = GetActualHandle()->GetSegmentCount(&count);

    error_code = MakeComErrorCode(result);
	if (IsSucceeded(error_code)) {
		return count;
	}
	else {
		return 0;
	}
}


const GeometrySink PathGeometry::Open(std::error_code& error_code) {

	ID2D1GeometrySink* sink_handle = nullptr;
	HRESULT result = GetActualHandle()->Open(&sink_handle);

    error_code = MakeComErrorCode(result);
    return GeometrySink(sink_handle);
}

}