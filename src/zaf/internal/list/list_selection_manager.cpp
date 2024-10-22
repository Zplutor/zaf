#include <zaf/internal/list/list_selection_manager.h>
#include <zaf/control/list_control.h>
#include <zaf/internal/list/list_control_part_context.h>
#include <zaf/internal/list/list_extended_multiple_selection_strategy.h>
#include <zaf/internal/list/list_single_selection_strategy.h>
#include <zaf/internal/list/list_simple_multiple_selection_strategy.h>

namespace zaf::internal {

zaf::SelectionMode ListSelectionManager::SelectionMode() const noexcept {
    return selection_mode_;
}


void ListSelectionManager::SetSelectionMode(zaf::SelectionMode mode) {

    selection_mode_ = mode;
    ReviseSelectionBySelectionMode();
    ResetSelectionStrategy();
}


void ListSelectionManager::ReviseSelectionBySelectionMode() {

    if (selection_mode_ == SelectionMode::None) {
        UnselectAllItems();
    }
    else if (selection_mode_ == SelectionMode::Single) {

        auto& selection_store = Context().SelectionStore();
        if (selection_store.GetAllSelectedCount() <= 1) {
            return;
        }

        auto first_selected_index = selection_store.GetFirstSelectedIndex();
        if (!first_selected_index) {
            return;
        }

        selection_store.ReplaceSelection(*first_selected_index, 1);
    }
}


void ListSelectionManager::ResetSelectionStrategy() {

    selection_strategy_ = [this]() -> std::unique_ptr<ListSelectionStrategy> {

        auto context = &Context();

        switch (selection_mode_) {
        case SelectionMode::Single:
            return std::make_unique<ListSingleSelectionStrategy>(context);

        case SelectionMode::SimpleMultiple:
            return std::make_unique<ListSimpleMultipleSelectionStrategy>(context);

        case SelectionMode::ExtendedMultiple:
            return std::make_unique<ListExtendedMultipleSelectionStrategy>(context);

        default:
            return std::make_unique<ListNoSelectionStrategy>(context);
        }
    }();
}


ListSelectionStrategy& ListSelectionManager::SelectionStrategy() const noexcept {
    return *selection_strategy_;
}


void ListSelectionManager::SelectAllItems() {

    auto data_source = Context().Owner().DataSource();
    if (!data_source) {
        return;
    }

    auto all_item_count = data_source->GetDataCount();

    auto& selection_store = Context().SelectionStore();
    auto selected_count = selection_store.GetAllSelectedCount();
    if (all_item_count == selected_count) {
        return;
    }

    selection_store.ReplaceSelection(0, all_item_count);
}


void ListSelectionManager::UnselectAllItems() {

    auto& selection_store = Context().SelectionStore();
    auto selected_count = selection_store.GetAllSelectedCount();
    if (selected_count == 0) {
        return;
    }

    selection_store.ReplaceSelection(0, 0);
}


void ListSelectionManager::SelectItemAtIndex(std::size_t index) {

    auto data_source = Context().Owner().DataSource();
    if (!data_source) {
        return;
    }

    ZAF_EXPECT(index < data_source->GetDataCount());

    auto& selection_store = Context().SelectionStore();
    if (selection_store.IsIndexSelected(index)) {
        return;
    }

    switch (selection_mode_) {

    case SelectionMode::Single:
        selection_store.ReplaceSelection(index, 1);
        break;

    case SelectionMode::SimpleMultiple:
    case SelectionMode::ExtendedMultiple:
        selection_store.AddSelection(index, 1);
        break;

    default:
        break;
    }
}


void ListSelectionManager::UnselectItemAtIndex(std::size_t index) {

    auto data_source = Context().Owner().DataSource();
    if (!data_source) {
        return;
    }

    ZAF_EXPECT(index < data_source->GetDataCount());

    auto& selection_store = Context().SelectionStore();
    if (!selection_store.IsIndexSelected(index)) {
        return;
    }

    selection_store.RemoveSelection(index, 1);
}

}