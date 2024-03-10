#pragma once

#include <optional>
#include <set>
#include <zaf/control/textual/dynamic_inline_object.h>
#include <zaf/internal/textual/text_box_module.h>
#include <zaf/internal/textual/text_model.h>
#include <zaf/rx/subscription_host.h>

namespace zaf::internal {

class TextBoxInlineObjectManager : public TextBoxModule, public SubscriptionHost {
public:
    explicit TextBoxInlineObjectManager(TextBoxModuleContext* context);

    void Initialize() override;

    bool IsInlineObjectSelected(const textual::DynamicInlineObject& object);

private:
    std::set<const textual::DynamicInlineObject*> FindSelectedInlineObjects() const;
    void OnTextModelChanged(const TextModelChangedInfo& event_info);
    void ResetCachedSelectedObjects();

private:
    std::optional<std::set<const textual::DynamicInlineObject*>> cached_selected_objects_;
};

}