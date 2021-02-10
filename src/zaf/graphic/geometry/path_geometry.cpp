#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/geometry/geometry_sink.h>

namespace zaf {

std::size_t PathGeometry::GetFigureCount() const {

	std::size_t count = 0;
	HRESULT result = GetActualHandle()->GetFigureCount(&count);

	ZAF_THROW_IF_COM_ERROR(result);
	return count;
}


std::size_t PathGeometry::GetSegmentCount() const {

	std::size_t count = 0;
	HRESULT result = GetActualHandle()->GetSegmentCount(&count);

	ZAF_THROW_IF_COM_ERROR(result);
	return count;
}


GeometrySink PathGeometry::Open() {

	ID2D1GeometrySink* sink_handle = nullptr;
	HRESULT result = GetActualHandle()->Open(&sink_handle);

	ZAF_THROW_IF_COM_ERROR(result);
    return GeometrySink(sink_handle, coordinate_origin_, aligned_coordinate_origin_);
}

}