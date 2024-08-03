#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/style/style.h>

namespace zaf {

class Control;

class StyleCollection : NonCopyableNonMovable {
public:
    explicit StyleCollection(Control* owner);

    void Add(std::shared_ptr<Style> style);
    void Clear();

    std::size_t Count() const noexcept {
        return styles_.size();
    }

    const std::shared_ptr<Style>& operator[](std::size_t index) const {
        return styles_[index];
    }

private:
    Control* owner_{};
    std::vector<std::shared_ptr<Style>> styles_;
};

}