#pragma once

namespace zaf::internal {

enum class ListSelectionChangeReason {
    ItemChange,
    ReplaceSelection,
    AddSelection,
    RemoveSelection,
};

}