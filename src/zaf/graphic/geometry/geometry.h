#pragma once

#include <memory>
#include <zaf/base/direct2d.h>
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
        const std::shared_ptr<GeometrySink>& sink
    ) {

        geometry1->GetHandle()->CombineWithGeometry(
            geometry2->GetHandle(),
            static_cast<D2D1_COMBINE_MODE>(combine_mode),
            nullptr,
            sink->GetHandle()
        );
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