#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/internal/list/list_input_handler.h>
#include <zaf/internal/list/list_item_height_manager.h>
#include <zaf/internal/list/list_selection_manager.h>
#include <zaf/internal/list/list_selection_store.h>

namespace zaf::internal {

class ListControlCore;

class ListControlPartContext : NonCopyableNonMovable {
public:
    explicit ListControlPartContext(ListControlCore* owner);

    ListControlCore& Owner() const {
        return *owner_;
    }

    ListInputHandler& InputHandler() const {
        return *input_handler_;
    }

    ListItemHeightManager& ItemHeightManager() const {
        return *item_height_manager_;
    }

    ListSelectionManager& SelectionManager() const {
        return *selection_manager_;
    }

    ListSelectionStore& SelectionStore() const {
        return *selection_store_;
    }

private:
    ListControlCore* owner_;

    std::unique_ptr<ListInputHandler> input_handler_;
    std::unique_ptr<ListItemHeightManager> item_height_manager_;
    std::unique_ptr<ListSelectionManager> selection_manager_;
    std::unique_ptr<ListSelectionStore> selection_store_;
};

}