#pragma once

#include <zaf/base/direct2d.h>

namespace zaf {

class Brush {
public:
	virtual ~Brush() {

		if (handle_ != nullptr) {
			handle_->Release();
		}
	}

	float GetOpacity() const {
		return handle_->GetOpacity();
	}

	void SetOpacity(float opacity) {
		handle_->SetOpacity(opacity);
	}

	ID2D1Brush* GetHandle() const {
		return handle_;
	}

protected:
	explicit Brush(ID2D1Brush* handle) : handle_(handle) { }

private:
	Brush(const Brush&) = delete;
	Brush& operator=(const Brush&) = delete;

private:
	ID2D1Brush* handle_;
};

}