#include <zaf/graphic/internal/snapped_path_geometry.h>
#include <new>
#include <utility>
#include <zaf/graphic/internal/snapped_geometry_sink.h>

namespace zaf::internal {

SnappedPathGeometry::SnappedPathGeometry(
    COMPtr<ID2D1PathGeometry> inner,
    const GeometrySnapData& snap_data) noexcept
    :
    inner_(std::move(inner)),
    snap_data_(snap_data) {

}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::QueryInterface(
    REFIID riid, 
    void** object) noexcept {

    if (!object) {
        return E_INVALIDARG;
    }

    if (riid == IID_IUnknown ||
        riid == __uuidof(ID2D1Resource) ||
        riid == __uuidof(ID2D1Geometry) ||
        riid == __uuidof(ID2D1PathGeometry) ||
        riid == IID_SnappedPathGeometry) {

        *object = this;
        AddRef();
        return S_OK;
    }

    *object = nullptr;
    return E_NOINTERFACE;
}


ULONG STDMETHODCALLTYPE SnappedPathGeometry::AddRef() noexcept {
    return ++reference_count_;
}


ULONG STDMETHODCALLTYPE SnappedPathGeometry::Release() noexcept {

    auto new_reference_count = --reference_count_;
    if (new_reference_count == 0) {
        delete this;
    }
    return new_reference_count;
}


void STDMETHODCALLTYPE SnappedPathGeometry::GetFactory(ID2D1Factory** factory) const noexcept {
    inner_->GetFactory(factory);
}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::GetBounds(
    const D2D1_MATRIX_3X2_F* world_transform,
    D2D1_RECT_F* bounds) const noexcept {

    return inner_->GetBounds(world_transform, bounds);
}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::GetWidenedBounds(
    FLOAT stroke_width,
    ID2D1StrokeStyle* stroke_style,
    const D2D1_MATRIX_3X2_F* world_transform,
    FLOAT flattening_tolerance,
    D2D1_RECT_F* bounds) const noexcept {

    return inner_->GetWidenedBounds(
        stroke_width,
        stroke_style,
        world_transform,
        flattening_tolerance,
        bounds);
}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::StrokeContainsPoint(
    D2D1_POINT_2F point,
    FLOAT stroke_width,
    ID2D1StrokeStyle* stroke_style,
    const D2D1_MATRIX_3X2_F* world_transform,
    FLOAT flattening_tolerance,
    BOOL* contains) const noexcept {

    return inner_->StrokeContainsPoint(
        point,
        stroke_width,
        stroke_style,
        world_transform,
        flattening_tolerance,
        contains);
}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::FillContainsPoint(
    D2D1_POINT_2F point,
    const D2D1_MATRIX_3X2_F* world_transform,
    FLOAT flattening_tolerance,
    BOOL* contains) const noexcept {

    return inner_->FillContainsPoint(point, world_transform, flattening_tolerance, contains);
}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::CompareWithGeometry(
    ID2D1Geometry* input_geometry,
    const D2D1_MATRIX_3X2_F* input_geometry_transform,
    FLOAT flattening_tolerance,
    D2D1_GEOMETRY_RELATION* relation) const noexcept {

    return inner_->CompareWithGeometry(
        input_geometry,
        input_geometry_transform,
        flattening_tolerance,
        relation);
}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::Simplify(
    D2D1_GEOMETRY_SIMPLIFICATION_OPTION simplification_option,
    const D2D1_MATRIX_3X2_F* world_transform,
    FLOAT flattening_tolerance,
    ID2D1SimplifiedGeometrySink* geometry_sink) const noexcept {

    return inner_->Simplify(
        simplification_option,
        world_transform,
        flattening_tolerance,
        geometry_sink);
}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::Tessellate(
    const D2D1_MATRIX_3X2_F* world_transform,
    FLOAT flattening_tolerance,
    ID2D1TessellationSink* tessellation_sink) const noexcept {

    return inner_->Tessellate(world_transform, flattening_tolerance, tessellation_sink);
}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::CombineWithGeometry(
    ID2D1Geometry* input_geometry,
    D2D1_COMBINE_MODE combine_mode,
    const D2D1_MATRIX_3X2_F* input_geometry_transform,
    FLOAT flattening_tolerance,
    ID2D1SimplifiedGeometrySink* geometry_sink) const noexcept {

    return inner_->CombineWithGeometry(
        input_geometry,
        combine_mode,
        input_geometry_transform,
        flattening_tolerance,
        geometry_sink);
}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::Outline(
    const D2D1_MATRIX_3X2_F* world_transform,
    FLOAT flattening_tolerance,
    ID2D1SimplifiedGeometrySink* geometry_sink) const noexcept {

    return inner_->Outline(world_transform, flattening_tolerance, geometry_sink);
}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::ComputeArea(
    const D2D1_MATRIX_3X2_F* world_transform,
    FLOAT flattening_tolerance,
    FLOAT* area) const noexcept {

    return inner_->ComputeArea(world_transform, flattening_tolerance, area);
}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::ComputeLength(
    const D2D1_MATRIX_3X2_F* world_transform,
    FLOAT flattening_tolerance,
    FLOAT* length) const noexcept {

    return inner_->ComputeLength(world_transform, flattening_tolerance, length);
}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::ComputePointAtLength(
    FLOAT length,
    const D2D1_MATRIX_3X2_F* world_transform,
    FLOAT flattening_tolerance,
    D2D1_POINT_2F* point,
    D2D1_POINT_2F* unit_tangent_vector) const noexcept {

    return inner_->ComputePointAtLength(
        length,
        world_transform,
        flattening_tolerance,
        point,
        unit_tangent_vector);
}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::Widen(
    FLOAT stroke_width,
    ID2D1StrokeStyle* stroke_style,
    const D2D1_MATRIX_3X2_F* world_transform,
    FLOAT flattening_tolerance,
    ID2D1SimplifiedGeometrySink* geometry_sink) const noexcept {

    return inner_->Widen(
        stroke_width,
        stroke_style,
        world_transform,
        flattening_tolerance,
        geometry_sink);
}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::Open(ID2D1GeometrySink** geometry_sink) noexcept {

    if (!geometry_sink) {
        return E_INVALIDARG;
    }

    *geometry_sink = nullptr;

    COMPtr<ID2D1GeometrySink> inner_sink;
    HRESULT result = inner_->Open(inner_sink.Reset());
    if (FAILED(result)) {
        return result;
    }

    auto snapped_sink = new (std::nothrow) SnappedGeometrySink(std::move(inner_sink), snap_data_);
    if (!snapped_sink) {
        return E_OUTOFMEMORY;
    }

    *geometry_sink = snapped_sink;
    return S_OK;
}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::Stream(
    ID2D1GeometrySink* geometry_sink) const noexcept {

    return inner_->Stream(geometry_sink);
}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::GetSegmentCount(UINT32* count) const noexcept {
    return inner_->GetSegmentCount(count);
}


HRESULT STDMETHODCALLTYPE SnappedPathGeometry::GetFigureCount(UINT32* count) const noexcept {
    return inner_->GetFigureCount(count);
}

}
