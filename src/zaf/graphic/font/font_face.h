#pragma once

#include <dwrite.h>
#include <cstdint>
#include <memory>
#include <zaf/base/assert.h>
#include <zaf/graphic/font/font_style.h>

namespace zaf {

class FontFamily;

class FontFace {
public:
	explicit FontFace(IDWriteFont* font_handle) : 
		font_handle_(font_handle),
		font_face_handle_(nullptr) {

		ZAF_ASSERT(font_handle_ != nullptr);
	}

	~FontFace() {

		font_handle_->Release();

		if (font_face_handle_ != nullptr) {
			font_face_handle_->Release();
		}
	}

	IDWriteFont* GetHandle() const {
		return font_handle_;
	}

	std::shared_ptr<FontFamily> GetFontFamily() const;

	int GetWeight() const {
		return font_handle_->GetWeight();
	}

	FontStyle GetStyle() const {
		return static_cast<FontStyle>(font_handle_->GetStyle());
	}

	bool HasCharacter(std::uint32_t unicode_character) const {
		BOOL has_character = FALSE;
		font_handle_->HasCharacter(unicode_character, &has_character);
		return has_character != FALSE;
	}

	bool IsSymbolFont() const {
		return font_handle_->IsSymbolFont() != FALSE;
	}

private:
	void CreateFontFaceHandle() {
		if (font_face_handle_ == nullptr) {
			font_handle_->CreateFontFace(&font_face_handle_);
		}
	}

private:
	IDWriteFont* font_handle_;
	IDWriteFontFace* font_face_handle_;
};

}