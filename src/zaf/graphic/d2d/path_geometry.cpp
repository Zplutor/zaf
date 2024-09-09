#include <zaf/graphic/d2d/path_geometry.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/d2d/geometry_sink.h>

namespace zaf::d2d {

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

    if (alignment_info_) {
        return GeometrySink{ sink_inner, *alignment_info_ };
    }
    else {
        return GeometrySink{ sink_inner };
    }
}

}