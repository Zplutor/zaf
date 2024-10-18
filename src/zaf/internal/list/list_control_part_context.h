#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/internal/list/list_input_handler.h>
#include <zaf/internal/list/list_item_height_manager.h>

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

private:
    ListControlCore* owner_;

    std::unique_ptr<ListInputHandler> input_handler_;
    std::unique_ptr<ListItemHeightManager> item_height_manager_;
};

}