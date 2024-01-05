#pragma once

#include <zaf/base/com_ptr.h>
#include <zaf/base/direct2d.h>
#include <zaf/graphic/point.h>

namespace zaf::internal {

class AlignedPathGeometry : public ID2D1PathGeometry {
public:
    AlignedPathGeometry(
        COMPtr<ID2D1PathGeometry> impl,
        const Point& coordinate_origin,
        const Point& aligned_coordinate_origin,
        float dpi);

    //IUnknown
    ULONG STDMETHODCALLTYPE AddRef(void) override;
    ULONG STDMETHODCALLTYPE Release(void) override;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;

    //ID2D1Resource
    STDMETHOD_(void, GetFactory)(_Outptr_ ID2D1Factory** factory) CONST override;

    //ID2D1Geometry
    STDMETHOD(GetBounds)(
        _In_opt_ CONST D2D1_MATRIX_3X2_F* worldTransform,
        _Out_ D2D1_RECT_F* bounds) CONST override;

    STDMETHOD(GetWidenedBounds)(
        FLOAT strokeWidth,
        _In_opt_ ID2D1StrokeStyle* strokeStyle,
        _In_opt_ CONST D2D1_MATRIX_3X2_F* worldTransform,
        FLOAT flatteningTolerance,
        _Out_ D2D1_RECT_F* bounds) CONST override;

    STDMETHOD(StrokeContainsPoint)(
        D2D1_POINT_2F point,
        FLOAT strokeWidth,
        _In_opt_ ID2D1StrokeStyle* strokeStyle,
        _In_opt_ CONST D2D1_MATRIX_3X2_F* worldTransform,
        FLOAT flatteningTolerance,
        _Out_ BOOL* contains) CONST override;

    STDMETHOD(FillContainsPoint)(
        D2D1_POINT_2F point,
        _In_opt_ CONST D2D1_MATRIX_3X2_F* worldTransform,
        FLOAT flatteningTolerance,
        _Out_ BOOL* contains) CONST override;

    STDMETHOD(CompareWithGeometry)(
        _In_ ID2D1Geometry* inputGeometry,
        _In_opt_ CONST D2D1_MATRIX_3X2_F* inputGeometryTransform,
        FLOAT flatteningTolerance,
        _Out_ D2D1_GEOMETRY_RELATION* relation) CONST override;

    STDMETHOD(Simplify)(
        D2D1_GEOMETRY_SIMPLIFICATION_OPTION simplificationOption,
        _In_opt_ CONST D2D1_MATRIX_3X2_F* worldTransform,
        FLOAT flatteningTolerance,
        _In_ ID2D1SimplifiedGeometrySink* geometrySink) CONST override;

    STDMETHOD(Tessellate)(
        _In_opt_ CONST D2D1_MATRIX_3X2_F* worldTransform,
        FLOAT flatteningTolerance,
        _In_ ID2D1TessellationSink* tessellationSink) CONST override;

    STDMETHOD(CombineWithGeometry)(
        _In_ ID2D1Geometry* inputGeometry,
        D2D1_COMBINE_MODE combineMode,
        _In_opt_ CONST D2D1_MATRIX_3X2_F* inputGeometryTransform,
        FLOAT flatteningTolerance,
        _In_ ID2D1SimplifiedGeometrySink* geometrySink) CONST override;

    STDMETHOD(Outline)(
        _In_opt_ CONST D2D1_MATRIX_3X2_F* worldTransform,
        FLOAT flatteningTolerance,
        _In_ ID2D1SimplifiedGeometrySink* geometrySink) CONST override;

    STDMETHOD(ComputeArea)(
        _In_opt_ CONST D2D1_MATRIX_3X2_F* worldTransform,
        FLOAT flatteningTolerance,
        _Out_ FLOAT* area) CONST override;

    STDMETHOD(ComputeLength)(
        _In_opt_ CONST D2D1_MATRIX_3X2_F* worldTransform,
        FLOAT flatteningTolerance,
        _Out_ FLOAT* length) CONST override;

    STDMETHOD(ComputePointAtLength)(
        FLOAT length,
        _In_opt_ CONST D2D1_MATRIX_3X2_F* worldTransform,
        FLOAT flatteningTolerance,
        _Out_opt_ D2D1_POINT_2F* point,
        _Out_opt_ D2D1_POINT_2F* unitTangentVector) CONST override;

    STDMETHOD(Widen)(
        FLOAT strokeWidth,
        _In_opt_ ID2D1StrokeStyle* strokeStyle,
        _In_opt_ CONST D2D1_MATRIX_3X2_F* worldTransform,
        FLOAT flatteningTolerance,
        _In_ ID2D1SimplifiedGeometrySink* geometrySink) CONST override;

    //ID2D1PathGeometry
    STDMETHOD(Open)(_COM_Outptr_ ID2D1GeometrySink** geometrySink) override;
    STDMETHOD(Stream)(_In_ ID2D1GeometrySink* geometrySink) CONST override;
    STDMETHOD(GetSegmentCount)(_Out_ UINT32* count) CONST override;
    STDMETHOD(GetFigureCount)(_Out_ UINT32* count) CONST override;

private:
    LONG reference_count_{ 1 };
    COMPtr<ID2D1PathGeometry> impl_;
    Point coordinate_origin_;
    Point aligned_coordinate_origin_;
    float dpi_{};
};

}