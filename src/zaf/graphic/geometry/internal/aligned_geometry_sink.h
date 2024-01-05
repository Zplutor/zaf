#pragma once

#include <zaf/base/com_ptr.h>
#include <zaf/base/direct2d.h>
#include <zaf/graphic/point.h>

namespace zaf::internal {

class AlignedGeometrySink : public ID2D1GeometrySink {
public:
    AlignedGeometrySink(
        COMPtr<ID2D1GeometrySink> impl,
        const Point& coordinate_origin,
        const Point& aligned_coordinate_origin,
        float dpi);

    //IUnknown
    ULONG STDMETHODCALLTYPE AddRef(void) override;
    ULONG STDMETHODCALLTYPE Release(void) override;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;

    //ID2D1SimplifiedGeometrySink
    STDMETHOD_(void, SetFillMode)(D2D1_FILL_MODE fillMode) override;
    STDMETHOD_(void, SetSegmentFlags)(D2D1_PATH_SEGMENT vertexFlags) override;

    STDMETHOD_(void, BeginFigure)(
        D2D1_POINT_2F startPoint,
        D2D1_FIGURE_BEGIN figureBegin) override;

    STDMETHOD_(void, AddLines)(
        _In_reads_(pointsCount) CONST D2D1_POINT_2F* points,
        UINT32 pointsCount) override;

    STDMETHOD_(void, AddBeziers)(
        _In_reads_(beziersCount) CONST D2D1_BEZIER_SEGMENT* beziers,
        UINT32 beziersCount) override;

    STDMETHOD_(void, EndFigure)(D2D1_FIGURE_END figureEnd) override;
    STDMETHOD(Close)() override;

    //ID2D1GeometrySink
    STDMETHOD_(void, AddLine)(D2D1_POINT_2F point) override;
    STDMETHOD_(void, AddBezier)(_In_ CONST D2D1_BEZIER_SEGMENT* bezier) override;
    STDMETHOD_(void, AddQuadraticBezier)(
        _In_ CONST D2D1_QUADRATIC_BEZIER_SEGMENT* bezier) override;

    STDMETHOD_(void, AddQuadraticBeziers)(
        _In_reads_(beziersCount) CONST D2D1_QUADRATIC_BEZIER_SEGMENT* beziers,
        UINT32 beziersCount) override;

    STDMETHOD_(void, AddArc)(_In_ CONST D2D1_ARC_SEGMENT* arc) override;

private:
    LONG reference_count_{ 1 };
    COMPtr<ID2D1GeometrySink> impl_;
    Point coordinate_origin_;
    Point aligned_coordinate_origin_;
    float dpi_{};
};

}