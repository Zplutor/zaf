#pragma once

#include <zaf/base/direct2d.h>

namespace zaf {

class Geometry {
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