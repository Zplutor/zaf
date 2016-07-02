#pragma once

#include <memory>
#include <zaf/base/direct2d.h>
#include <zaf/base/error.h>
#include <zaf/graphic/geometry/geometry_sink.h>

namespace zaf {

class GeometrySink;

class Geometry {
public:
    enum class CombineMode {
        Union = D2D1_COMBINE_MODE_UNION,
        Intersect = D2D1_COMBINE_MODE_INTERSECT,
        Xor = D2D1_COMBINE_MODE_XOR,
        Exclude = D2D1_COMBINE_MODE_EXCLUDE,
    };

public:
    static void Combine(
        const std::shared_ptr<Geometry>& geometry1,
        const std::shared_ptr<Geometry>& geometry2,
        CombineMode combine_mode,
        const std::shared_ptr<GeometrySink>& sink) {

        std::error_code error_code;
        Combine(geometry1, geometry2, combine_mode, sink, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    static void Combine(
        const std::shared_ptr<Geometry>& geometry1,
        const std::shared_ptr<Geometry>& geometry2,
        CombineMode combine_mode,
        const std::shared_ptr<GeometrySink>& sink,
        std::error_code& error_code) {

        HRESULT result = geometry1->GetHandle()->CombineWithGeometry(
            geometry2->GetHandle(),
            static_cast<D2D1_COMBINE_MODE>(combine_mode),
            nullptr,
            sink->GetHandle());

        error_code = MakeComErrorCode(result);
    }

public:
	virtual ~Geometry() {
		if (handle_ != nullptr) {
			handle_->Release();
		}
	}

	ID2D1Geometry* GetHandle() const {
		return handle_;
	}

protected:
	explicit Geometry(ID2D1Geometry* handle) : handle_(handle) { }

private:
	Geometry(const Geometry&) = delete;
	Geometry& operator=(const Geometry&) = delete;

private:
	ID2D1Geometry* handle_;
};

}