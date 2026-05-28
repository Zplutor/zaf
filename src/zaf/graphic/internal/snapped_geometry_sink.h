#pragma once

#include <atomic>
#include <zaf/base/com_ptr.h>
#include <zaf/base/direct2d.h>
#include <zaf/graphic/internal/geometry_snap_data.h>

namespace zaf::internal {

class SnappedGeometrySink : public ID2D1GeometrySink {
public:
    SnappedGeometrySink(
        COMPtr<ID2D1GeometrySink> inner, 
        const GeometrySnapData& snap_data) noexcept;

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object) noexcept override;
    ULONG STDMETHODCALLTYPE AddRef() noexcept override;
    ULONG STDMETHODCALLTYPE Release() noexcept override;

    void STDMETHODCALLTYPE SetFillMode(D2D1_FILL_MODE fill_mode) noexcept override;
    void STDMETHODCALLTYPE SetSegmentFlags(D2D1_PATH_SEGMENT vertex_flags) noexcept override;
    void STDMETHODCALLTYPE BeginFigure(
        D2D1_POINT_2F start_point,
        D2D1_FIGURE_BEGIN figure_begin) noexcept override;
    void STDMETHODCALLTYPE AddLines(
        const D2D1_POINT_2F* points,
        UINT points_count) noexcept override;
    void STDMETHODCALLTYPE AddBeziers(
        const D2D1_BEZIER_SEGMENT* beziers,
        UINT beziers_count) noexcept override;
    void STDMETHODCALLTYPE EndFigure(D2D1_FIGURE_END figure_end) noexcept override;
    HRESULT STDMETHODCALLTYPE Close() noexcept override;
    void STDMETHODCALLTYPE AddLine(D2D1_POINT_2F point) noexcept override;
    void STDMETHODCALLTYPE AddBezier(const D2D1_BEZIER_SEGMENT* bezier) noexcept override;
    void STDMETHODCALLTYPE AddQuadraticBezier(
        const D2D1_QUADRATIC_BEZIER_SEGMENT* bezier) noexcept override;
    void STDMETHODCALLTYPE AddQuadraticBeziers(
        const D2D1_QUADRATIC_BEZIER_SEGMENT* beziers,
        UINT beziers_count) noexcept override;
    void STDMETHODCALLTYPE AddArc(const D2D1_ARC_SEGMENT* arc) noexcept override;

private:
    D2D1_POINT_2F SnapPoint(const D2D1_POINT_2F& point) const noexcept;
    D2D1_BEZIER_SEGMENT SnapBezier(const D2D1_BEZIER_SEGMENT& bezier) const noexcept;
    D2D1_QUADRATIC_BEZIER_SEGMENT SnapQuadraticBezier(
        const D2D1_QUADRATIC_BEZIER_SEGMENT& bezier) const noexcept;

private:
    std::atomic<ULONG> reference_count_{ 1 };
    COMPtr<ID2D1GeometrySink> inner_;
    GeometrySnapData snap_data_;
};

}