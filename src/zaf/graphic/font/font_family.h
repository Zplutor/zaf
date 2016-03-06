#pragma once

#include <dwrite.h>
#include <cstdint>

namespace zaf {

class FontFamily {
public:
	explicit FontFamily(IDWriteFontFamily* handle) : handle_(handle) { }

	~FontFamily() {
		handle_->Release();
	}

	std::size_t GetFontCount() const {
		return handle_->GetFontCount();
	}

private:
	FontFamily(const FontFamily&) = delete;
	FontFamily& operator=(const FontFamily&) = delete;

private:
	IDWriteFontFamily* handle_;
};

}