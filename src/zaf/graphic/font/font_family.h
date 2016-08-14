#pragma once

#include <dwrite.h>
#include <cstdint>

namespace zaf {

/**
 Represents a family of related fonts.
 */
class FontFamily {
public:
    /**
     Construct the instance with specified handle.

     The instance takes over the lifetime of handle, and would release it when destroyed.
     */
	explicit FontFamily(IDWriteFontFamily* handle) : handle_(handle) { }

	~FontFamily() {
        if (handle_ != nullptr) {
		    handle_->Release();
        }
	}

    /**
     Get the number of fonts.
     */
	std::size_t GetFontCount() const {
		return handle_->GetFontCount();
	}

	FontFamily(const FontFamily&) = delete;
	FontFamily& operator=(const FontFamily&) = delete;

private:
	IDWriteFontFamily* handle_;
};

}