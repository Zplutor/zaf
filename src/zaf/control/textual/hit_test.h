#pragma once

namespace zaf::textual {

class HitTestInfo {
public:
    explicit HitTestInfo(bool is_mouse_inside) : is_mouse_inside_(is_mouse_inside) {

    }

    bool IsMouseInside() const {
        return is_mouse_inside_;
    }

private:
    bool is_mouse_inside_{};
};


enum class HitTestResult {
    None,
    Object,
};

}