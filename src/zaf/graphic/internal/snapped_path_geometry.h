#pragma once

#include <atomic>
#include <zaf/base/com_ptr.h>
#include <zaf/base/direct2d.h>
#include <zaf/graphic/internal/geometry_snap_data.h>

namespace zaf::internal {

constexpr GUID IID_SnappedPathGeometry = {
	0xe15d1456, 0x1a45, 0x4fc4, { 0xb6, 0x8f, 0x11, 0x7a, 0xc0, 0xf5, 0x4f, 0x73 }
};

class SnappedPathGeometry : public ID2D1PathGeometry {
public:
	SnappedPathGeometry(
		COMPtr<ID2D1PathGeometry> inner, 
		const GeometrySnapData& snap_data) noexcept;

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object) noexcept override;
	ULONG STDMETHODCALLTYPE AddRef() noexcept override;
	ULONG STDMETHODCALLTYPE Release() noexcept override;

	void STDMETHODCALLTYPE GetFactory(ID2D1Factory** factory) const noexcept override;

	HRESULT STDMETHODCALLTYPE GetBounds(
		const D2D1_MATRIX_3X2_F* world_transform,
		D2D1_RECT_F* bounds) const noexcept override;

	HRESULT STDMETHODCALLTYPE GetWidenedBounds(
		FLOAT stroke_width,
		ID2D1StrokeStyle* stroke_style,
		const D2D1_MATRIX_3X2_F* world_transform,
		FLOAT flattening_tolerance,
		D2D1_RECT_F* bounds) const noexcept override;

	HRESULT STDMETHODCALLTYPE StrokeContainsPoint(
		D2D1_POINT_2F point,
		FLOAT stroke_width,
		ID2D1StrokeStyle* stroke_style,
		const D2D1_MATRIX_3X2_F* world_transform,
		FLOAT flattening_tolerance,
		BOOL* contains) const noexcept override;

	HRESULT STDMETHODCALLTYPE FillContainsPoint(
		D2D1_POINT_2F point,
		const D2D1_MATRIX_3X2_F* world_transform,
		FLOAT flattening_tolerance,
		BOOL* contains) const noexcept override;

	HRESULT STDMETHODCALLTYPE CompareWithGeometry(
		ID2D1Geometry* input_geometry,
		const D2D1_MATRIX_3X2_F* input_geometry_transform,
		FLOAT flattening_tolerance,
		D2D1_GEOMETRY_RELATION* relation) const noexcept override;

	HRESULT STDMETHODCALLTYPE Simplify(
		D2D1_GEOMETRY_SIMPLIFICATION_OPTION simplification_option,
		const D2D1_MATRIX_3X2_F* world_transform,
		FLOAT flattening_tolerance,
		ID2D1SimplifiedGeometrySink* geometry_sink) const noexcept override;

	HRESULT STDMETHODCALLTYPE Tessellate(
		const D2D1_MATRIX_3X2_F* world_transform,
		FLOAT flattening_tolerance,
		ID2D1TessellationSink* tessellation_sink) const noexcept override;

	HRESULT STDMETHODCALLTYPE CombineWithGeometry(
		ID2D1Geometry* input_geometry,
		D2D1_COMBINE_MODE combine_mode,
		const D2D1_MATRIX_3X2_F* input_geometry_transform,
		FLOAT flattening_tolerance,
		ID2D1SimplifiedGeometrySink* geometry_sink) const noexcept override;

	HRESULT STDMETHODCALLTYPE Outline(
		const D2D1_MATRIX_3X2_F* world_transform,
		FLOAT flattening_tolerance,
		ID2D1SimplifiedGeometrySink* geometry_sink) const noexcept override;

	HRESULT STDMETHODCALLTYPE ComputeArea(
		const D2D1_MATRIX_3X2_F* world_transform,
		FLOAT flattening_tolerance,
		FLOAT* area) const noexcept override;

	HRESULT STDMETHODCALLTYPE ComputeLength(
		const D2D1_MATRIX_3X2_F* world_transform,
		FLOAT flattening_tolerance,
		FLOAT* length) const noexcept override;

	HRESULT STDMETHODCALLTYPE ComputePointAtLength(
		FLOAT length,
		const D2D1_MATRIX_3X2_F* world_transform,
		FLOAT flattening_tolerance,
		D2D1_POINT_2F* point,
		D2D1_POINT_2F* unit_tangent_vector) const noexcept override;

	HRESULT STDMETHODCALLTYPE Widen(
		FLOAT stroke_width,
		ID2D1StrokeStyle* stroke_style,
		const D2D1_MATRIX_3X2_F* world_transform,
		FLOAT flattening_tolerance,
		ID2D1SimplifiedGeometrySink* geometry_sink) const noexcept override;

	HRESULT STDMETHODCALLTYPE Open(ID2D1GeometrySink** geometry_sink) noexcept override;
	HRESULT STDMETHODCALLTYPE Stream(ID2D1GeometrySink* geometry_sink) const noexcept override;
	HRESULT STDMETHODCALLTYPE GetSegmentCount(UINT32* count) const noexcept override;
	HRESULT STDMETHODCALLTYPE GetFigureCount(UINT32* count) const noexcept override;

	const COMPtr<ID2D1PathGeometry>& Inner() const noexcept {
		return inner_;
	}

private:
	std::atomic<ULONG> reference_count_{ 1 };
	COMPtr<ID2D1PathGeometry> inner_;
	GeometrySnapData snap_data_;

};

}