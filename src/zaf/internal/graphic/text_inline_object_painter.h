#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/graphic/canvas.h>

namespace zaf::internal {

class TextInlineObjectPainter : NonCopyableNonMovable {
public:
    class Guard : NonCopyable {
    public:
        explicit Guard(TextInlineObjectPainter* painter) : painter_(painter) {

        }

        ~Guard() {
            if (painter_) {
                painter_->EndPaint();
            }
        }

        Guard(Guard&& other) noexcept : painter_(other.painter_) {
            other.painter_ = nullptr;
        }

        Guard& operator=(Guard&& other) noexcept {
            painter_ = other.painter_;
            other.painter_ = nullptr;
            return *this;
        }

    private:
        TextInlineObjectPainter* painter_{};
    };

public:
    Guard BeginPaint(Canvas& canvas) {
        canvas_ = &canvas;
        return Guard{ this };
    }

    zaf::Canvas* Canvas() const {
        return canvas_;
    }

private:
    void EndPaint() {
        canvas_ = nullptr;
    }

private:
    zaf::Canvas* canvas_{};
};

}