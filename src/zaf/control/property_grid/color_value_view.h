#pragma once

#include <zaf/control/property_grid/value_view.h>
#include <zaf/control/rich_edit.h>

namespace zaf::property_grid {

class ColorValueView : public ValueView {
public:
    void SetAccessMethod(AccessMethod) override;
    void SetValue(const std::shared_ptr<Object>& object) override;

protected:
    void Initialize() override;

private:
    class ColorSquare : public Control {
    public:
        const Color& Color() const {
            return color_;
        }

        void SetColor(const zaf::Color& color);

    protected:
        void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const override;

    private:
        void PaintTransparentColorSmallSquares(Canvas& canvas) const;

    private:
        zaf::Color color_;
    };

private:
    void InitializeColorSquare();
    void InitializeRichEdit();
    void ChangeColorByEdit();
    void SetColorToEdit(const Color& color);

private:
    std::shared_ptr<ColorSquare> color_square_;
    std::shared_ptr<RichEdit> color_edit_;
};

}