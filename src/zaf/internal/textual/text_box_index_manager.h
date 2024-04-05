#pragma once

#include <zaf/internal/textual/text_box_module.h>

namespace zaf::internal {

/**
Provides common index related logic interfaces.
*/
class TextBoxIndexManager : public TextBoxModule {
public:
    explicit TextBoxIndexManager(TextBoxModuleContext* context);

    void Initialize() override;

    std::size_t GetPreviousIndex(std::size_t index) const;
    std::size_t GetNextIndex(std::size_t index) const;
};

}