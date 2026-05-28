#include <zaf/graphic/internal/snapped_geometry_sink.h>
#include <utility>
#include <zaf/graphic/internal/snapping_helper.h>

namespace zaf::internal {

SnappedGeometrySink::SnappedGeometrySink(
    COMPtr<ID2D1GeometrySink> inner,
    const GeometrySnapData& snap_data) noexcept
    :
    inner_(std::move(inner)),
    snap_data_(snap_data) {

}


HRESULT STDMETHODCALLTYPE SnappedGeometrySink::QueryInterface(
    REFIID riid, 
    void** object) noexcept {

    if (!object) {
        return E_INVALIDARG;
    }

    if (riid == IID_IUnknown ||
        riid == __uuidof(ID2D1SimplifiedGeometrySink) ||
        riid == __uuidof(ID2D1GeometrySink)) {

        *object = this;
        AddRef();
        return S_OK;
    }

    *object = nullptr;
    return E_NOINTERFACE;
}


ULONG STDMETHODCALLTYPE SnappedGeometrySink::AddRef() noexcept {
    return ++reference_count_;
}


ULONG STDMETHODCALLTYPE SnappedGeometrySink::Release() noexcept {

    auto new_reference_count = --reference_count_;
    if (new_reference_count == 0) {
        delete this;
    }
    return new_reference_count;
}


void STDMETHODCALLTYPE SnappedGeometrySink::SetFillMode(D2D1_FILL_MODE fill_mode) noexcept {
    inner_->SetFillMode(fill_mode);
}


void STDMETHODCALLTYPE SnappedGeometrySink::SetSegmentFlags(D2D1_PATH_SEGMENT vertex_flags) noexcept {
    inner_->SetSegmentFlags(vertex_flags);
}


void STDMETHODCALLTYPE SnappedGeometrySink::BeginFigure(
    D2D1_POINT_2F start_point,
    D2D1_FIGURE_BEGIN figure_begin) noexcept {

    inner_->BeginFigure(SnapPoint(start_point), figure_begin);
}


void STDMETHODCALLTYPE SnappedGeometrySink::AddLines(
    const D2D1_POINT_2F* points,
    UINT points_count) noexcept {

    if (!points || points_count == 0) {
        return;
    }

    for (UINT point_index = 0; point_index < points_count; ++point_index) {
        inner_->AddLine(SnapPoint(points[point_index]));
    }
}


void STDMETHODCALLTYPE SnappedGeometrySink::AddBeziers(
    const D2D1_BEZIER_SEGMENT* beziers,
    UINT beziers_count) noexcept {

    if (!beziers || beziers_count == 0) {
        return;
    }

    for (UINT bezier_index = 0; bezier_index < beziers_count; ++bezier_index) {
        auto snapped_bezier = SnapBezier(beziers[bezier_index]);
        inner_->AddBezier(&snapped_bezier);
    }
}


void STDMETHODCALLTYPE SnappedGeometrySink::EndFigure(D2D1_FIGURE_END figure_end) noexcept {
    inner_->EndFigure(figure_end);
}


HRESULT STDMETHODCALLTYPE SnappedGeometrySink::Close() noexcept {
    return inner_->Close();
}


void STDMETHODCALLTYPE SnappedGeometrySink::AddLine(D2D1_POINT_2F point) noexcept {
    inner_->AddLine(SnapPoint(point));
}


void STDMETHODCALLTYPE SnappedGeometrySink::AddBezier(const D2D1_BEZIER_SEGMENT* bezier) noexcept {

    if (!bezier) {
        return;
    }

    auto snapped_bezier = SnapBezier(*bezier);
    inner_->AddBezier(&snapped_bezier);
}


void STDMETHODCALLTYPE SnappedGeometrySink::AddQuadraticBezier(
    const D2D1_QUADRATIC_BEZIER_SEGMENT* bezier) noexcept {

    if (!bezier) {
        return;
    }

    auto snapped_bezier = SnapQuadraticBezier(*bezier);
    inner_->AddQuadraticBezier(&snapped_bezier);
}


void STDMETHODCALLTYPE SnappedGeometrySink::AddQuadraticBeziers(
    const D2D1_QUADRATIC_BEZIER_SEGMENT* beziers,
    UINT beziers_count) noexcept {

    if (!beziers || beziers_count == 0) {
        return;
    }

    for (UINT bezier_index = 0; bezier_index < beziers_count; ++bezier_index) {
        auto snapped_bezier = SnapQuadraticBezier(beziers[bezier_index]);
        inner_->AddQuadraticBezier(&snapped_bezier);
    }
}


void STDMETHODCALLTYPE SnappedGeometrySink::AddArc(const D2D1_ARC_SEGMENT* arc) noexcept {

    if (!arc) {
        return;
    }

    D2D1_ARC_SEGMENT snapped_arc = *arc;
    snapped_arc.point = SnapPoint(arc->point);
    inner_->AddArc(&snapped_arc);
}


D2D1_POINT_2F SnappedGeometrySink::SnapPoint(const D2D1_POINT_2F& point) const noexcept {

    auto snapped_point = SnapInCoordinateSystem(
        Point::FromD2D1POINT2F(point),
        snap_data_.stroke_width,
        snap_data_.dpi,
        snap_data_.coordinate_origin,
        snap_data_.snapped_coordinate_origin);

    return snapped_point.ToD2D1POINT2F();
}


D2D1_BEZIER_SEGMENT SnappedGeometrySink::SnapBezier(
    const D2D1_BEZIER_SEGMENT& bezier) const noexcept {

    D2D1_BEZIER_SEGMENT snapped_bezier = bezier;
    snapped_bezier.point1 = SnapPoint(bezier.point1);
    snapped_bezier.point2 = SnapPoint(bezier.point2);
    snapped_bezier.point3 = SnapPoint(bezier.point3);
    return snapped_bezier;
}


D2D1_QUADRATIC_BEZIER_SEGMENT SnappedGeometrySink::SnapQuadraticBezier(
    const D2D1_QUADRATIC_BEZIER_SEGMENT& bezier) const noexcept {

    D2D1_QUADRATIC_BEZIER_SEGMENT snapped_bezier = bezier;
    snapped_bezier.point1 = SnapPoint(bezier.point1);
    snapped_bezier.point2 = SnapPoint(bezier.point2);
    return snapped_bezier;
}

}
