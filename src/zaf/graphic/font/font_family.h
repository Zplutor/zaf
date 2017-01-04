#pragma once

#include <dwrite.h>
#include <cstdint>
#include <memory>
#include <zaf/base/error.h>

namespace zaf {

class FontCollection;
class FontFace;
class LocalizedStrings;

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
     Get the font collection that contains the fonts.

     @return
         Return nullptr if failed.
     */
    const std::shared_ptr<FontCollection> GetFontCollection(std::error_code& error_code) const;

    const std::shared_ptr<FontCollection> GetFontCollection() const {
        std::error_code error_code;
        auto result = GetFontCollection(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Get the number of fonts.
     */
	std::size_t GetFontCount() const {
		return handle_->GetFontCount();
	}

    /**
     Gets a font given its zero-based index.

     @param index
         Zero-based index of the font in the font family.
    
     @return
         Return nullptr if failed.
     */
    const std::shared_ptr<FontFace> GetFont(std::size_t index, std::error_code& error_code) const;

    const std::shared_ptr<FontFace> GetFont(std::size_t index) const {
        std::error_code error_code;
        auto result = GetFont(index, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Get a localized strings object that contains the family names for the font family, indexed by locale name.

     @return
         Return nullptr if failed.
     */
    const std::shared_ptr<LocalizedStrings> GetFamilyNames(std::error_code& error_code) const;

    const std::shared_ptr<LocalizedStrings> GetFamilyNames() const {
        std::error_code error_code;
        auto result = GetFamilyNames(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    IDWriteFontFamily* GetHandle() const {
        return handle_;
    }

	FontFamily(const FontFamily&) = delete;
	FontFamily& operator=(const FontFamily&) = delete;

private:
	IDWriteFontFamily* handle_;
};

}