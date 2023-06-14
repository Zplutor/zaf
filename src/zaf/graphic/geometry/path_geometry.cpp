#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/geometry/geometry_sink.h>

namespace zaf {

std::size_t PathGeometry::GetFigureCount() const {

    UINT32 count = 0;
    HRESULT result = Inner()->GetFigureCount(&count);

    ZAF_THROW_IF_COM_ERROR(result);
    return count;
}


std::size_t PathGeometry::GetSegmentCount() const {

    UINT32 count = 0;
    HRESULT result = Inner()->GetSegmentCount(&count);

    ZAF_THROW_IF_COM_ERROR(result);
    return count;
}


GeometrySink PathGeometry::Open() {

    COMPtr<ID2D1GeometrySink> sink_inner;
    HRESULT result = Inner()->Open(sink_inner.Reset());

    ZAF_THROW_IF_COM_ERROR(result);
    return GeometrySink(sink_inner, coordinate_origin_, aligned_coordinate_origin_);
}

}