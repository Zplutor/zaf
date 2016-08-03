#pragma once

#include <zaf/base/direct2d.h>

namespace zaf {

/**
 Represents a brush.

 This a base class of all concrete brush.
 */
class Brush {
public:
	virtual ~Brush() {

		if (handle_ != nullptr) {
			handle_->Release();
		}
	}

    /**
     Get opacity.
     */
	float GetOpacity() const {
		return handle_->GetOpacity();
	}

    /**
     Set opacity.
     */
	void SetOpacity(float opacity) {
		handle_->SetOpacity(opacity);
	}

    /**
     Get brush's handle.
     */
	ID2D1Brush* GetHandle() const {
		return handle_;
	}

    Brush(const Brush&) = delete;
    Brush& operator=(const Brush&) = delete;

protected:
    /**
     Construct the instance with specified handle.

     The brush instance takes over the lifetime of handle. It would
     release the handle when destroyed.
     */
	explicit Brush(ID2D1Brush* handle) : handle_(handle) { }

private:
	ID2D1Brush* handle_;
};

}