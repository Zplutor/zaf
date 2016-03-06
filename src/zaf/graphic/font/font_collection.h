#pragma once

#include <dwrite.h>
#include <memory>
#include <string>

namespace zaf {

class FontFamily;

class FontCollection {
public:
	explicit FontCollection(IDWriteFontCollection* handle) : handle_(handle) { }

	~FontCollection() {
		handle_->Release();
	}

	IDWriteFontCollection* GetHandle() const {
		return handle_;
	}

	std::size_t GetFontFamilyCount() const {
		return handle_->GetFontFamilyCount();
	}

	std::shared_ptr<FontFamily> GetFontFamily(std::size_t index) const;

	std::shared_ptr<FontFamily> FindFontFamily(const std::wstring& family_name) const;

private:
	FontCollection(const FontCollection&) = delete;
	FontCollection& operator=(const FontCollection&) = delete;

private:
	IDWriteFontCollection* handle_;
};

}