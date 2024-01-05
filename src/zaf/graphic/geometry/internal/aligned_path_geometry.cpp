#include <zaf/graphic/geometry/internal/aligned_path_geometry.h>
#include <zaf/graphic/geometry/internal/aligned_geometry_sink.h>

namespace zaf::internal {

AlignedPathGeometry::AlignedPathGeometry(
    COMPtr<ID2D1PathGeometry> impl,
    const Point& coordinate_origin,
    const Point& aligned_coordinate_origin,
    float dpi) 
    :
    impl_(std::move(impl)),
    coordinate_origin_(coordinate_origin),
    aligned_coordinate_origin_(aligned_coordinate_origin),
    dpi_(dpi) {

}


ULONG __stdcall AlignedPathGeometry::AddRef(void) {
    return InterlockedIncrement(&reference_count_);
}


ULONG __stdcall AlignedPathGeometry::Release(void) {
    ULONG result = InterlockedDecrement(&reference_count_);
    if (result == 0) {
        delete this;
    }
    return result;
}


HRESULT __stdcall AlignedPathGeometry::QueryInterface(REFIID riid, void** ppvObject) {
    return impl_->QueryInterface(riid, ppvObject);
}


STDMETHODIMP_(void __stdcall) AlignedPathGeometry::GetFactory(ID2D1Factory** factory) const {
    return impl_->GetFactory(factory);
}


STDMETHODIMP_(HRESULT __stdcall) AlignedPathGeometry::GetBounds(
    const D2D1_MATRIX_3X2_F* worldTransform,
    D2D1_RECT_F* bounds) const {

    return impl_->GetBounds(worldTransform, bounds);
}


STDMETHODIMP_(HRESULT __stdcall) AlignedPathGeometry::GetWidenedBounds(
    FLOAT strokeWidth, 
    ID2D1StrokeStyle* strokeStyle, 
    const D2D1_MATRIX_3X2_F* worldTransform,
    FLOAT flatteningTolerance,
    D2D1_RECT_F* bounds) const {

    return impl_->GetWidenedBounds(
        strokeWidth,
        strokeStyle,
        worldTransform,
        flatteningTolerance,
        bounds);
}


STDMETHODIMP_(HRESULT __stdcall) AlignedPathGeometry::StrokeContainsPoint(
    D2D1_POINT_2F point, 
    FLOAT strokeWidth, 
    ID2D1StrokeStyle* strokeStyle, 
    const D2D1_MATRIX_3X2_F* worldTransform,
    FLOAT flatteningTolerance, 
    BOOL* contains) const {

    return impl_->StrokeContainsPoint(
        point,
        strokeWidth,
        strokeStyle,
        worldTransform,
        flatteningTolerance,
        contains);
}


STDMETHODIMP_(HRESULT __stdcall) AlignedPathGeometry::FillContainsPoint(
    D2D1_POINT_2F point,
    const D2D1_MATRIX_3X2_F* worldTransform, 
    FLOAT flatteningTolerance,
    BOOL* contains) const {

    return impl_->FillContainsPoint(point, worldTransform, flatteningTolerance, contains);
}


STDMETHODIMP_(HRESULT __stdcall) AlignedPathGeometry::CompareWithGeometry(
    ID2D1Geometry* inputGeometry, 
    const D2D1_MATRIX_3X2_F* inputGeometryTransform, 
    FLOAT flatteningTolerance,
    D2D1_GEOMETRY_RELATION* relation) const {

    return impl_->CompareWithGeometry(
        inputGeometry,
        inputGeometryTransform,
        flatteningTolerance,
        relation);
}


STDMETHODIMP_(HRESULT __stdcall) AlignedPathGeometry::Simplify(
    D2D1_GEOMETRY_SIMPLIFICATION_OPTION simplificationOption, 
    const D2D1_MATRIX_3X2_F* worldTransform, 
    FLOAT flatteningTolerance, 
    ID2D1SimplifiedGeometrySink* geometrySink) const {

    return impl_->Simplify(
        simplificationOption,
        worldTransform, 
        flatteningTolerance,
        geometrySink);
}


STDMETHODIMP_(HRESULT __stdcall) AlignedPathGeometry::Tessellate(
    const D2D1_MATRIX_3X2_F* worldTransform,
    FLOAT flatteningTolerance, 
    ID2D1TessellationSink* tessellationSink) const {

    return impl_->Tessellate(worldTransform, flatteningTolerance, tessellationSink);
}


STDMETHODIMP_(HRESULT __stdcall) AlignedPathGeometry::CombineWithGeometry(
    ID2D1Geometry* inputGeometry, 
    D2D1_COMBINE_MODE combineMode,
    const D2D1_MATRIX_3X2_F* inputGeometryTransform,
    FLOAT flatteningTolerance, 
    ID2D1SimplifiedGeometrySink* geometrySink) const {

    return CombineWithGeometry(
        inputGeometry,
        combineMode,
        inputGeometryTransform,
        flatteningTolerance,
        geometrySink);
}


STDMETHODIMP_(HRESULT __stdcall) AlignedPathGeometry::Outline(
    const D2D1_MATRIX_3X2_F* worldTransform,
    FLOAT flatteningTolerance, 
    ID2D1SimplifiedGeometrySink* geometrySink) const {

    return impl_->Outline(worldTransform, flatteningTolerance, geometrySink);
}


STDMETHODIMP_(HRESULT __stdcall) AlignedPathGeometry::ComputeArea(
    const D2D1_MATRIX_3X2_F* worldTransform,
    FLOAT flatteningTolerance, 
    FLOAT* area) const {

    return ComputeArea(worldTransform, flatteningTolerance, area);
}


STDMETHODIMP_(HRESULT __stdcall) AlignedPathGeometry::ComputeLength(
    const D2D1_MATRIX_3X2_F* worldTransform, 
    FLOAT flatteningTolerance, 
    FLOAT* length) const {

    return ComputeLength(worldTransform, flatteningTolerance, length);
}


STDMETHODIMP_(HRESULT __stdcall) AlignedPathGeometry::ComputePointAtLength(
    FLOAT length,
    const D2D1_MATRIX_3X2_F* worldTransform,
    FLOAT flatteningTolerance,
    D2D1_POINT_2F* point,
    D2D1_POINT_2F* unitTangentVector) const {

    return ComputePointAtLength(
        length,
        worldTransform,
        flatteningTolerance,
        point, 
        unitTangentVector);
}


STDMETHODIMP_(HRESULT __stdcall) AlignedPathGeometry::Widen(
    FLOAT strokeWidth, 
    ID2D1StrokeStyle* strokeStyle,
    const D2D1_MATRIX_3X2_F* worldTransform,
    FLOAT flatteningTolerance,
    ID2D1SimplifiedGeometrySink* geometrySink) const {

    return impl_->Widen(
        strokeWidth,
        strokeStyle,
        worldTransform, 
        flatteningTolerance, 
        geometrySink);
}


STDMETHODIMP_(HRESULT __stdcall) AlignedPathGeometry::Open(ID2D1GeometrySink** geometrySink) {

    COMPtr<ID2D1GeometrySink> sink_impl;
    HRESULT hresult = impl_->Open(sink_impl.Reset());
    if (FAILED(hresult)) {
        return hresult;
    }

    *geometrySink = new AlignedGeometrySink{ 
        std::move(sink_impl), 
        coordinate_origin_,
        aligned_coordinate_origin_,
        dpi_,
    };
    return S_OK;
}


STDMETHODIMP_(HRESULT __stdcall) AlignedPathGeometry::Stream(
    ID2D1GeometrySink* geometrySink) const {

    return impl_->Stream(geometrySink);
}


STDMETHODIMP_(HRESULT __stdcall) AlignedPathGeometry::GetSegmentCount(UINT32* count) const {
    return impl_->GetSegmentCount(count);
}


STDMETHODIMP_(HRESULT __stdcall) AlignedPathGeometry::GetFigureCount(UINT32* count) const {
    return impl_->GetFigureCount(count);
}

}