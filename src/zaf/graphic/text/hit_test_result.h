#pragma once

#include <dwrite.h>

namespace zaf {

class HitTestMetrics : private DWRITE_HIT_TEST_METRICS {
public:
    std::size_t TextIndex() const {
        return textPosition;
    }

    std::size_t Length() const {
        return length;
    }

    float Left() const {
        return left;
    }

    float Top() const {
        return top;
    }

    float Width() const {
        return width;
    }

    float Height() const {
        return height;
    }
};


class HitTestIndexResult {
public:
    struct Data {
        float x{};
        float y{};
        HitTestMetrics metrics;
    };

public:
    float X() const {
        return inner_.x;
    }

    float Y() const {
        return inner_.y;
    }

    const HitTestMetrics& Metrics() const {
        return inner_.metrics;
    }

    Data& Inner() {
        return inner_;
    }

private:
    Data inner_;
};


class HitTestPointResult {
public:
    struct Data {
        BOOL is_trailing_hit{};
        BOOL is_inside{};
        HitTestMetrics metrics;
    };

public:
    bool IsTrailingHit() const {
        return !!inner_.is_trailing_hit;
    }

    bool IsInside() const {
        return !!inner_.is_inside;
    }

    const HitTestMetrics& Metrics() const {
        return inner_.metrics;
    }

    Data& Inner() {
        return inner_;
    }

private:
    Data inner_;
};

}