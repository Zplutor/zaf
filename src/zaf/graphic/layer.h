#pragma once

#include <zaf/base/direct2d.h>
#include <zaf/base/rect.h>

namespace zaf {

class Layer {
public:
	class Parameters {
	public:
		Parameters() : opacity(1) { }

		Rect content_bounds;
		float opacity;
	};

public:
	Layer(ID2D1Layer* handle) : handle_(handle) { }

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