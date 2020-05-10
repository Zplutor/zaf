#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <zaf/base/error.h>
#include <zaf/graphic/brush/brush.h>
#include <zaf/graphic/font/font_style.h>
#include <zaf/graphic/text/line_metrics.h>
#include <zaf/graphic/text/text_format.h>
#include <zaf/graphic/text/text_metrics.h>
#include <zaf/graphic/text/text_range.h>

namespace zaf {

/**
 Represents a block of text after it has been fully analyzed and formatted.

 You should create TextLayout instances via GraphicFactory::CreateTextLayout.
 */
class TextLayout : public TextFormat {
public:
    TextLayout() { }

    /**
     Create the instance with specified handle.

     See also TextFormat::TextFormat.
     */
	explicit TextLayout(IDWriteTextLayout* handle) : TextFormat(handle) { }

    /**
     Get the layout maximum width.
     */
	float GetMaxWidth() const {
		return GetHandle()->GetMaxWidth();
	}

    /**
     Set the layout maximum width.
     */
	void SetMaxWidth(float max_width, std::error_code& error_code) {
		HRESULT result = GetHandle()->SetMaxWidth(max_width);
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
		return GetHandle()->GetMaxHeight();
	}

    /**
     Set the layout maximum height.
     */
	void SetMaxHeight(float max_height, std::error_code& error_code) {
		HRESULT result = GetHandle()->SetMaxHeight(max_height);
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

     @param font_family_name
         The font family name that applies to the entire text string within the range specified by range.

     @param range
         Text range to which this change applies.

     @param error_code
         An output parameter indicates the error, if any.
     */
    void SetFontFamilyName(const std::wstring& font_family_name, const TextRange& range, std::error_code& error_code) {
        HRESULT result = GetHandle()->SetFontFamilyName(font_family_name.c_str(), range.ToDWRITETEXTRANGE());
        error_code = MakeComErrorCode(result);
    }

    void SetFontFamilyName(const std::wstring& font_family_name, const TextRange& range) {
        std::error_code error_code;
        SetFontFamilyName(font_family_name, range, error_code);
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

     @param size
         The font size to be set for text in the range specified by range.

     @param range
         Text range to which this change applies.

     @param error_code
         An output parameter indicates the error, if any.
     */
    void SetFontSize(float size, const TextRange& range, std::error_code& error_code) {
		HRESULT result = GetHandle()->SetFontSize(size, range.ToDWRITETEXTRANGE());
        error_code = MakeComErrorCode(result);
	}

    void SetFontSize(float size, const TextRange& range) {
        std::error_code error_code;
        SetFontSize(size, range, error_code);
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

     @param font_style
         The font style to be set for text within a range specified by range.

     @param range
         The text range to which this change applies.

     @param error_code
         An output parameter indicates the error, if any.
     */
    void SetFontStyle(FontStyle font_style, const TextRange& range, std::error_code& error_code) {
		HRESULT result = GetHandle()->SetFontStyle(static_cast<DWRITE_FONT_STYLE>(font_style), range.ToDWRITETEXTRANGE());
        error_code = MakeComErrorCode(result);
	}

    void SetFontStyle(FontStyle font_style, const TextRange& range) {
        std::error_code error_code;
        SetFontStyle(font_style, range, error_code);
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

     @param weight
         The font weight to be set for text within the range specified by range.

     @param range
         Text range to which this change applies.

     @param error_code
         An output parameter indicates the error, if any.
     */
    void SetFontWeight(int weight, const TextRange& range, std::error_code& error_code) {
		HRESULT result = GetHandle()->SetFontWeight(static_cast<DWRITE_FONT_WEIGHT>(weight), range.ToDWRITETEXTRANGE());
        error_code = MakeComErrorCode(result);
	}

    void SetFontWeight(int weight, const TextRange& range) {
        std::error_code error_code;
        SetFontWeight(weight, range, error_code);
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

     @param has_underline
         A bool flag that indicates whether underline takes place within a specified text range.

     @param range
         Text range to which this change applies.

     @param error_code
         An output parameter indicates the error, if any.
     */
    void SetHasUnderline(bool has_underline, const TextRange& range, std::error_code& error_code) {
		HRESULT result = GetHandle()->SetUnderline(has_underline ? TRUE : FALSE, range.ToDWRITETEXTRANGE());
        error_code = MakeComErrorCode(result);
	}

    void SetHasUnderline(bool has_underline, const TextRange& range) {
        std::error_code error_code;
        SetHasUnderline(has_underline, range, error_code);
        ZAF_CHECK_ERROR(error_code);
    }

    /**
     Get the drawing brush at the specified text position.

     @param position
         The position of the text whose drawing brush is to be retrieved.

     @param range
         Contains the range of text that has the same formatting as the text at the position specified 
         by position. This means the run has the exact formatting as the position specified, including 
         but not limited to the drawing effect. This parameter can be nullptr.

     @param error_code
         An output parameter indicates the error, if any.

     @return
         Return an empty brush if error occurs.
     */
    const Brush GetBrush(std::size_t position, TextRange* range, std::error_code& error_code);

    const Brush GetBrush(std::size_t position, TextRange* range) {
        std::error_code error_code;
        auto result = GetBrush(position, range);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    /**
     Set brush for text within a specified text range.

     @param brush
         The brush to apply to text.

     @param range
         Text range to which this change applies.

     @param error_code
         An output parameter indicates the error, if any.
     */
    void SetBrush(const Brush& brush, const TextRange& range, std::error_code& error_code) {
        HRESULT result = GetHandle()->SetDrawingEffect(brush.GetHandle(), range.ToDWRITETEXTRANGE());
        error_code = MakeComErrorCode(result);
    }

    void SetBrush(const Brush& brush, const TextRange& range) {
        std::error_code error_code;
        SetBrush(brush, range, error_code);
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
	TextMetrics GetMetrics(std::error_code& error_code) const;

    TextMetrics GetMetrics() const {
        std::error_code error_code;
        auto result = GetMetrics(error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    IDWriteTextLayout* GetHandle() const {
        return static_cast<IDWriteTextLayout*>(__super::GetHandle());
    }
};

}