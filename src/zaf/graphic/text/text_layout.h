#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <zaf/base/error/com_error.h>
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
    void SetMaxWidth(float max_width) {
        HRESULT result = GetHandle()->SetMaxWidth(max_width);
        ZAF_THROW_IF_COM_ERROR(result);
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
    void SetMaxHeight(float max_height) {
        HRESULT result = GetHandle()->SetMaxHeight(max_height);
        ZAF_THROW_IF_COM_ERROR(result);
    }

    /**
     Get the font family name of the text at the specified position.

     @param position
         The position of the text to examine.

     @param range
         The range of text that has the same formatting as the text at the position specified by position. 
         This means the run has the exact formatting as the position specified, including but not limited to 
         the font family name. This parameter can be nullptr.
     */
    std::wstring GetFontFamilyName(std::size_t position, TextRange* range) const;

    /**
     Set font family name for text within a specified text range.

     @param font_family_name
         The font family name that applies to the entire text string within the range specified by range.

     @param range
         Text range to which this change applies.
     */
    void SetFontFamilyName(const std::wstring& font_family_name, const TextRange& range) {
        HRESULT result = GetHandle()->SetFontFamilyName(font_family_name.c_str(), range.ToDWRITETEXTRANGE());
        ZAF_THROW_IF_COM_ERROR(result);
    }

    /**
     Get the font size of the text at the specified position.

     @param position
         The position of the text to inspect.

     @param range
         The range of text that has the same formatting as the text at the position specified by position. 
         This means the run has the exact formatting as the position specified, including but not limited to the
         font size. This parameter can be nullptr.
     */
    float GetFontSize(std::size_t position, TextRange* range) const;

    /**
     Set the font size for text within a specified text range.

     @param size
         The font size to be set for text in the range specified by range.

     @param range
         Text range to which this change applies.
     */
    void SetFontSize(float size, const TextRange& range) {
        HRESULT result = GetHandle()->SetFontSize(size, range.ToDWRITETEXTRANGE());
        ZAF_THROW_IF_COM_ERROR(result);
    }

    /**
     Get the font style (also known as slope) of the text at the specified position.

     @param position
         The position of the text to inspect.

     @param range
         The range of text that has the same formatting as the text at the position specified by position.
         This means the run has the exact formatting as the position specified, including but not limited 
         to the font style. This parameter can be nullptr.
     */
    FontStyle GetFontStyle(std::size_t position, TextRange* range) const;

    /**
     Set the font style for text within a text range.

     @param font_style
         The font style to be set for text within a range specified by range.

     @param range
         The text range to which this change applies.
     */
    void SetFontStyle(FontStyle font_style, const TextRange& range) {
        HRESULT result = GetHandle()->SetFontStyle(static_cast<DWRITE_FONT_STYLE>(font_style), range.ToDWRITETEXTRANGE());
        ZAF_THROW_IF_COM_ERROR(result);
    }

    /**
     Get the font weight of the text at the specified position.

     @param position
         The position of the text to inspect.

     @param range
         The range of text that has the same formatting as the text at the position specified by position.
         This means the run has the exact formatting as the position specified, including but not limited 
         to the font weight. This parameter can be nullptr.
     */
    int GetFontWeight(std::size_t position, TextRange* range) const;

    /**
     Set the font weight for text within a text range.

     @param weight
         The font weight to be set for text within the range specified by range.

     @param range
         Text range to which this change applies.
     */
    void SetFontWeight(int weight, const TextRange& range) {
        HRESULT result = GetHandle()->SetFontWeight(static_cast<DWRITE_FONT_WEIGHT>(weight), range.ToDWRITETEXTRANGE());
        ZAF_THROW_IF_COM_ERROR(result);
    }

    /**
     Get the underline presence of the text at the specified position.

     @param positon
         The current text position.

     @param range
         The range of text that has the same formatting as the text at the position specified by position.
         This means the run has the exact formatting as the position specified, including but not limited 
         to the underline. This parameter can be nullptr.
     */
    bool HasUnderline(std::size_t position, TextRange* range) const;

    /**
     Set underlining for text within a specified text range.

     @param has_underline
         A bool flag that indicates whether underline takes place within a specified text range.

     @param range
         Text range to which this change applies.
     */
    void SetHasUnderline(bool has_underline, const TextRange& range) {
        HRESULT result = GetHandle()->SetUnderline(has_underline ? TRUE : FALSE, range.ToDWRITETEXTRANGE());
        ZAF_THROW_IF_COM_ERROR(result);
    }

    /**
     Get the drawing brush at the specified text position.

     @param position
         The position of the text whose drawing brush is to be retrieved.

     @param range
         Contains the range of text that has the same formatting as the text at the position specified 
         by position. This means the run has the exact formatting as the position specified, including 
         but not limited to the drawing effect. This parameter can be nullptr.
     */
    Brush GetBrush(std::size_t position, TextRange* range);

    /**
     Set brush for text within a specified text range.

     @param brush
         The brush to apply to text.

     @param range
         Text range to which this change applies.
     */
    void SetBrush(const Brush& brush, const TextRange& range) {
        HRESULT result = GetHandle()->SetDrawingEffect(brush.GetHandle(), range.ToDWRITETEXTRANGE());
        ZAF_THROW_IF_COM_ERROR(result);
    }

    /**
     Retrieves the information about each individual text line of the text string.

     @param max_line_count
         The maximum number of lines to retrieves.
     */
    std::vector<LineMetrics> GetLineMetrics(std::size_t max_line_count) const;

    /**
     Retrieves overall metrics for the formatted text.
     */
    TextMetrics GetMetrics() const;

    IDWriteTextLayout* GetHandle() const {
        return static_cast<IDWriteTextLayout*>(__super::GetHandle());
    }
};

}