#pragma once

#include <zaf/control/text_box.h>

namespace zaf::internal {

/**
Provides common index related logic interfaces.
*/
class TextBoxIndexManager {
public:
    explicit TextBoxIndexManager(TextBox& owner);

    void Initialize();

    std::size_t GetBackwardIndex(std::size_t index) const;
    std::size_t GetForwardIndex(std::size_t index) const;

private:
    TextBox& owner_;
};

}
