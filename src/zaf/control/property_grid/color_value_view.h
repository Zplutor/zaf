#pragma once

#include <zaf/control/label.h>
#include <zaf/control/property_grid/value_view.h>

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
        void SetColor(const Color& color);

    protected:
        void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) override;

    private:
        void PaintTransparentColorSmallSquares(Canvas& canvas);

    private:
        Color color_;
    };

private:
    void SetLabelText(const Color& color);

private:
    std::shared_ptr<ColorSquare> color_square_;
    std::shared_ptr<Label> color_label_;
};

}