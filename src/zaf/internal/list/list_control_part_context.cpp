#include <zaf/internal/list/list_control_part_context.h>

namespace zaf::internal {

ListControlPartContext::ListControlPartContext(ListControlCore* owner) :
    owner_(owner),
    input_handler_(std::make_unique<ListInputHandler>(this)),
    item_height_manager_(std::make_unique<ListItemHeightManager>()),
    selection_manager_(std::make_unique<ListSelectionManager>(this)),
    selection_store_(std::make_unique<ListSelectionStore>()),
    visible_item_manager_(std::make_unique<ListVisibleItemManager>(this)) {

}

}