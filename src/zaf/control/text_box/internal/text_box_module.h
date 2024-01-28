#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf::internal {

class TextBoxModuleContext;

class TextBoxModule : NonCopyableNonMovable {
public:
    virtual ~TextBoxModule() = default;

protected:
    explicit TextBoxModule(TextBoxModuleContext* context);

    TextBoxModuleContext& Context() const {
        return *context_;
    }

private:
    TextBoxModuleContext* context_;
};

}