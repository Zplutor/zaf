#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <zaf/base/error.h>
#include <zaf/graphic/font/font_style.h>
#include <zaf/graphic/text/line_metrics.h>
#include <zaf/graphic/text/text_format.h>
#include <zaf/graphic/text/text_metrics.h>
#include <zaf/graphic/text/text_range.h>

namespace zaf {

/**
 Represents a block of text after it has been fully analyzed and formatted.

 You should create TextLayout instances via ResourceFactory::CreateTextLayout.
 */
class TextLayout : public TextFormat {
public:
    /**
     Create the instance with specified handle.

     See also TextFormat::TextFormat.
     */
	TextLayout(IDWriteTextLayout* handle) : 
		TextFormat(handle), 
		handle_(handle) { 
	
		ZAF_ASSERT(handle_ != nullptr);
	}

    /**
     Get the text layout handle.
     */
	IDWriteTextLayout* GetHandle() const {
		return handle_;
	}

    /**
     Get the layout maximum width.
     */
	float GetMaxWidth() const {
		return handle_->GetMaxWidth();
	}

    /**
     Set the layout maximum width.
     */
	void SetMaxWidth(float max_width, std::error_code& error_code) {
		HRESULT result = handle_->SetMaxWidth(max_width);
        error_code = MakeComErrorCode(result);
	}

    void SetMaxWidth(float max_width) {
        std::error_code error_code;
        SetMaxWidth(max_width, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    /**
     Get the layout maximum height.
     */
	float GetMaxHeight() const {
		return handle_->GetMaxHeight();
	}

    /**
     Set the layout maximum height.
     */
	void SetMaxHeight(float max_height, std::error_code& error_code) {
		HRESULT result = handle_->SetMaxHeight(max_height);
        error_code = MakeComErrorCode(result);
	}

    void SetMaxHeight(float max_height) {
        std::error_code error_code;
        SetMaxHeight(max_height, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    /**
     Get the font family name of the text at the specified position.

     @param position
         The position of the text to examine.

     @param range
         The range of text that has the same formatting as the text at the position specified by position. 
         This means the run has the exact formatting as the position specified, including but not limited to 
         the font family name. This parameter can be nullptr.

     @param error_code
         An output parameter indicates the error, if any.

     @return
         Return an empty string if error occurs.
     */
	std::wstring GetFontFamilyName(std::size_t position, TextRange* range, std::error_code& error_code) const;

    std::wstring GetFontFamilyName(std::size_t position, TextRange* range) const {
        std::error_code error_code;
        std::wstring result = GetFontFamilyName(position, range, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Set font family name for text within a specified text range.

     @param range
         Text range to which this change applies.

     @param font_family_name
         The font family name that applies to the entire text string within the range specified by range.

     @param error_code
         An output parameter indicates the error, if any.
     */
    void SetFontFamilyName(const TextRange& range, const std::wstring& font_family_name, std::error_code& error_code) {
        HRESULT result = handle_->SetFontFamilyName(font_family_name.c_str(), range.ToDWRITETEXTRANGE());
        error_code = MakeComErrorCode(result);
    }

    void SetFontFamilyName(const TextRange& range, const std::wstring& font_family_name) {
        std::error_code error_code;
        SetFontFamilyName(range, font_family_name, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    /**
     Get the font size of the text at the specified position.

     @param position
         The position of the text to inspect.

     @param range
         The range of text that has the same formatting as the text at the position specified by position. 
         This means the run has the exact formatting as the position specified, including but not limited to the
         font size. This parameter can be nullptr.

     @param error_code
         An output parameter indicates the error, if any.

     @return 
         Return 0 if error occurs.
     */
	float GetFontSize(std::size_t position, TextRange* range, std::error_code& error_code) const;

    float GetFontSize(std::size_t position, TextRange* range) const {
        std::error_code error_code;
        auto result = GetFontSize(position, range, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Set the font size for text within a specified text range.

     @param range
         Text range to which this change applies.

     @param size
         The font size to be set for text in the range specified by range.

     @param error_code
         An output parameter indicates the error, if any.
     */
	void SetFontSize(const TextRange& range, float size, std::error_code& error_code) {
		HRESULT result = handle_->SetFontSize(size, range.ToDWRITETEXTRANGE());
        error_code = MakeComErrorCode(result);
	}

    void SetFontSize(const TextRange& range, float size) {
        std::error_code error_code;
        SetFontSize(range, size, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    /**
     Get the font style (also known as slope) of the text at the specified position.

     @param position
         The position of the text to inspect.

     @param range
         The range of text that has the same formatting as the text at the position specified by position.
         This means the run has the exact formatting as the position specified, including but not limited 
         to the font style. This parameter can be nullptr.

     @param error_code
         An output parameter indicates the error, if any.

     @return
         Return FontStyle::Normal if error occurs.
     */
	FontStyle GetFontStyle(std::size_t position, TextRange* range, std::error_code& error_code) const;

    FontStyle GetFontStyle(std::size_t position, TextRange* range) const {
        std::error_code error_code;
        auto result = GetFontStyle(position, range, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Set the font style for text within a text range.

     @param range
         The text range to which this change applies.

     @param font_style
         The font style to be set for text within a range specified by range.

     @param error_code
         An output parameter indicates the error, if any.
     */
	void SetFontStyle(const TextRange& range, FontStyle font_style, std::error_code& error_code) {
		HRESULT result = handle_->SetFontStyle(static_cast<DWRITE_FONT_STYLE>(font_style), range.ToDWRITETEXTRANGE());
        error_code = MakeComErrorCode(result);
	}

    void SetFontStyle(const TextRange& range, FontStyle font_style) {
        std::error_code error_code;
        SetFontStyle(range, font_style, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    /**
     Get the font weight of the text at the specified position.

     @param position
         The position of the text to inspect.

     @param range
         The range of text that has the same formatting as the text at the position specified by position.
         This means the run has the exact formatting as the position specified, including but not limited 
         to the font weight. This parameter can be nullptr.

     @param error_code
         An output parameter indicates the error, if any.

     @return 
         Return 0 if error occurs.
     */
	int GetFontWeight(std::size_t position, TextRange* range, std::error_code& error_code) const;

    int GetFontWeight(std::size_t position, TextRange* range) const {
        std::error_code error_code;
        auto result = GetFontWeight(position, range, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Set the font weight for text within a text range.

     @param range
         Text range to which this change applies.

     @param weight
         The font weight to be set for text within the range specified by range.

     @param error_code
         An output parameter indicates the error, if any.
     */
	void SetFontWeight(const TextRange& range, int weight, std::error_code& error_code) {
		HRESULT result = handle_->SetFontWeight(static_cast<DWRITE_FONT_WEIGHT>(weight), range.ToDWRITETEXTRANGE());
        error_code = MakeComErrorCode(result);
	}

    void SetFontWeight(const TextRange& range, int weight) {
        std::error_code error_code;
        SetFontWeight(range, weight, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    /**
     Get the underline presence of the text at the specified position.

     @param positon
         The current text position.

     @param range
         The range of text that has the same formatting as the text at the position specified by position.
         This means the run has the exact formatting as the position specified, including but not limited 
         to the underline. This parameter can be nullptr.

     @param error_code
         An output parameter indicates the error, if any.

     @return
         Return false if error occurs.
     */
	bool HasUnderline(std::size_t position, TextRange* range, std::error_code& error_code) const;

    bool HasUnderline(std::size_t position, TextRange* range) const {
        std::error_code error_code;
        auto result = HasUnderline(position, range, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Set underlining for text within a specified text range.

     @param range
         Text range to which this change applies.

     @param has_underline
         A bool flag that indicates whether underline takes place within a specified text range.

     @param error_code
         An output parameter indicates the error, if any.
     */
	void SetHasUnderline(const TextRange& range, bool has_underline, std::error_code& error_code) {
		HRESULT result = handle_->SetUnderline(has_underline ? TRUE : FALSE, range.ToDWRITETEXTRANGE());
        error_code = MakeComErrorCode(result);
	}

    void SetHasUnderline(const TextRange& range, bool has_underline) {
        std::error_code error_code;
        SetHasUnderline(range, has_underline, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    /**
     Retrieves the information about each individual text line of the text string.

     @param max_line_count
         The maximum number of lines to retrieves.

     @param error_code
         An output parameter indicates the error, if any.

     @return 
        Return an empty vector if error occurs.
     */
	const std::vector<LineMetrics> GetLineMetrics(std::size_t max_line_count, std::error_code& error_code) const;

    const std::vector<LineMetrics> GetLineMetrics(std::size_t max_line_count) const {
        std::error_code error_code;
        auto result = GetLineMetrics(max_line_count, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Retrieves overall metrics for the formatted text.

     @param error_code
         An output parameter indicates the error, if any.

     @return 
        Return empty text metrics if error occurs.
     */
	const TextMetrics GetMetrics(std::error_code& error_code) const;

    const TextMetrics GetMetrics() const {
        std::error_code error_code;
        auto result = GetMetrics(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

private:
	IDWriteTextLayout* handle_;
};

}