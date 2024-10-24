#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/scroll_box.h>
#include <zaf/internal/list/list_control_core.h>
#include <zaf/internal/list/list_input_handler.h>
#include <zaf/internal/list/list_item_height_manager.h>
#include <zaf/internal/list/list_selection_manager.h>
#include <zaf/internal/list/list_selection_store.h>
#include <zaf/internal/list/list_visible_item_manager.h>

namespace zaf::internal {

class ListControlPartsContext : NonCopyableNonMovable {
public:
    explicit ListControlPartsContext(ScrollBox* owner);

    ScrollBox& Owner() const {
        return *owner_;
    }

    ListControlCore& Core() const {
        return *core_;
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

    ListVisibleItemManager& VisibleItemManager() const {
        return *visible_item_manager_;
    }

private:
    ScrollBox* owner_{};
    std::unique_ptr<ListControlCore> core_;
    std::unique_ptr<ListInputHandler> input_handler_;
    std::unique_ptr<ListItemHeightManager> item_height_manager_;
    std::unique_ptr<ListSelectionManager> selection_manager_;
    std::unique_ptr<ListSelectionStore> selection_store_;
    std::unique_ptr<ListVisibleItemManager> visible_item_manager_;
};

}