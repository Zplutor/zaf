#pragma once

#include <zaf/base/direct2d.h>
#include <zaf/graphic/size.h>

namespace zaf {

class Layer {
public:
	explicit Layer(ID2D1Layer* handle) : handle_(handle) { }

	~Layer() {
		if (handle_ != nullptr) {
			handle_->Release();
		}
	}

	const Size GetSize() const {
		return Size::FromD2D1SIZEF(handle_->GetSize());
	}

	ID2D1Layer* GetHandle() const {
		return handle_;
	}

private:
	ID2D1Layer* handle_;
};

}