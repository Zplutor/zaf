#pragma once

#include <zaf/base/direct2d.h>

namespace zaf {

class Brush {
public:
	Brush(ID2D1Brush* handle) : handle_(handle) { }

	virtual ~Brush() {

		if (handle_ != nullptr) {
			handle_->Release();
		}
	}

	ID2D1Brush* GetHandle() const {
		return handle_;
	}

private:
	Brush(const Brush&) = delete;
	Brush& operator=(const Brush&) = delete;

private:
	ID2D1Brush* handle_;
};

}