#include <zaf/graphic/geometry/internal/aligned_geometry_sink.h>
#include <zaf/base/range.h>
#include <zaf/graphic/internal/alignment_helper.h>
#include "aligned_geometry_sink.h"

namespace zaf::internal {

AlignedGeometrySink::AlignedGeometrySink(
    COMPtr<ID2D1GeometrySink> impl,
    const Point& coordinate_origin,
    const Point& aligned_coordinate_origin,
    float dpi)
    :
    impl_(std::move(impl)),
    coordinate_origin_(coordinate_origin),
    aligned_coordinate_origin_(aligned_coordinate_origin),
    dpi_(dpi) {

}


ULONG __stdcall AlignedGeometrySink::AddRef(void) {
    return InterlockedIncrement(&reference_count_);
}


ULONG __stdcall AlignedGeometrySink::Release(void) {
    ULONG result = InterlockedDecrement(&reference_count_);
    if (result == 0) {
        delete this;
    }
    return result;
}


HRESULT __stdcall AlignedGeometrySink::QueryInterface(REFIID riid, void** ppvObject) {
    return impl_->QueryInterface(riid, ppvObject);
}


STDMETHODIMP_(void __stdcall) AlignedGeometrySink::SetFillMode(D2D1_FILL_MODE fillMode) {
    impl_->SetFillMode(fillMode);
}


STDMETHODIMP_(void __stdcall) AlignedGeometrySink::SetSegmentFlags(D2D1_PATH_SEGMENT vertexFlags) {
    impl_->SetSegmentFlags(vertexFlags);
}


STDMETHODIMP_(void __stdcall) AlignedGeometrySink::BeginFigure(
    D2D1_POINT_2F startPoint,
    D2D1_FIGURE_BEGIN figureBegin) {

    auto aligned_point = AlignInRelatedCoordinateSystem(
        Point::FromD2D1POINT2F(startPoint),
        0,
        dpi_,
        coordinate_origin_, 
        aligned_coordinate_origin_);

    impl_->BeginFigure(aligned_point.ToD2D1POINT2F(), figureBegin);
}


STDMETHODIMP_(void __stdcall) AlignedGeometrySink::AddLines(
    const D2D1_POINT_2F* points, 
    UINT32 pointsCount) {

    std::vector<D2D1_POINT_2F> aligned_points;
    aligned_points.reserve(pointsCount);

    for (auto index : Range(0, pointsCount)) {

        auto aligned_point = AlignInRelatedCoordinateSystem(
            Point::FromD2D1POINT2F(points[index]),
            0,
            dpi_,
            coordinate_origin_,
            aligned_coordinate_origin_);

        aligned_points.push_back(aligned_point.ToD2D1POINT2F());
    }

    impl_->AddLines(aligned_points.data(), static_cast<UINT32>(aligned_points.size()));
}


STDMETHODIMP_(void __stdcall) AlignedGeometrySink::AddBeziers(
    const D2D1_BEZIER_SEGMENT* beziers, 
    UINT32 beziersCount) {

    impl_->AddBeziers(beziers, beziersCount);
}


STDMETHODIMP_(void __stdcall) AlignedGeometrySink::EndFigure(D2D1_FIGURE_END figureEnd) {
    impl_->EndFigure(figureEnd);
}


STDMETHODIMP_(HRESULT __stdcall) AlignedGeometrySink::Close() {
    return impl_->Close();
}


STDMETHODIMP_(void __stdcall) AlignedGeometrySink::AddLine(D2D1_POINT_2F point) {

    auto aligned_point = AlignInRelatedCoordinateSystem(
        Point::FromD2D1POINT2F(point),
        0,
        dpi_,
        coordinate_origin_,
        aligned_coordinate_origin_);

    impl_->AddLine(aligned_point.ToD2D1POINT2F());
}


STDMETHODIMP_(void __stdcall) AlignedGeometrySink::AddBezier(const D2D1_BEZIER_SEGMENT* bezier) {
    impl_->AddBezier(bezier);
}


STDMETHODIMP_(void __stdcall) AlignedGeometrySink::AddQuadraticBezier(
    const D2D1_QUADRATIC_BEZIER_SEGMENT* bezier) {

    impl_->AddQuadraticBezier(bezier);
}


STDMETHODIMP_(void __stdcall) AlignedGeometrySink::AddQuadraticBeziers(
    const D2D1_QUADRATIC_BEZIER_SEGMENT* beziers,
    UINT32 beziersCount) {

    impl_->AddQuadraticBeziers(beziers, beziersCount);
}


STDMETHODIMP_(void __stdcall) AlignedGeometrySink::AddArc(const D2D1_ARC_SEGMENT* arc) {
    impl_->AddArc(arc);
}

}