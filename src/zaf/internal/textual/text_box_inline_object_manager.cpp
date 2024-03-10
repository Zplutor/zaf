#include <zaf/internal/textual/text_box_inline_object_manager.h>
#include <zaf/internal/textual/text_box_module_context.h>
#include <zaf/internal/textual/text_box_selection_manager.h>

namespace zaf::internal {

TextBoxInlineObjectManager::TextBoxInlineObjectManager(TextBoxModuleContext* context) : 
    TextBoxModule(context) {

}


void TextBoxInlineObjectManager::Initialize() {

    Subscriptions() += Context().SelectionManager().SelectionChangedEvent().Subscribe(
        std::bind(&TextBoxInlineObjectManager::ResetCachedSelectedObjects, this));

    Subscriptions() += Context().TextModel().TextChangedEvent().Subscribe(
        std::bind(&TextBoxInlineObjectManager::OnTextModelChanged, this, std::placeholders::_1));
}


bool TextBoxInlineObjectManager::IsInlineObjectSelected(
    const textual::DynamicInlineObject& object) {

    if (!cached_selected_objects_) {
        cached_selected_objects_ = FindSelectedInlineObjects();
    }

    return cached_selected_objects_->contains(&object);
}


std::set<const textual::DynamicInlineObject*> 
    TextBoxInlineObjectManager::FindSelectedInlineObjects() const {

    std::set<const textual::DynamicInlineObject*> result;

    const auto& selection_range = Context().SelectionManager().SelectionRange();

    for (const auto& each_item : Context().TextModel().StyledText().InlineObjects()) {

        if (selection_range.Intersects(each_item.Range())) {

            auto dynamic_inline_object = 
                dynamic_cast<textual::DynamicInlineObject*>(each_item.InlineObject().get());

            if (dynamic_inline_object) {
                result.insert(dynamic_inline_object);
            }
        }
    }

    return result;
}


void TextBoxInlineObjectManager::OnTextModelChanged(const TextModelChangedInfo& event_info) {

    if (HasFlag(event_info.ChangedAttributes(), TextModelAttribute::Text) ||
        HasFlag(event_info.ChangedAttributes(), TextModelAttribute::InlineObject)) {

        ResetCachedSelectedObjects();
    }
}


void TextBoxInlineObjectManager::ResetCachedSelectedObjects() {
    cached_selected_objects_.reset();
}

}