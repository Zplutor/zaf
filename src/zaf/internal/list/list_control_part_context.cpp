#include <zaf/internal/list/list_control_part_context.h>

namespace zaf::internal {

ListControlPartContext::ListControlPartContext(ListControlCore* owner) :
    owner_(owner),
    input_handler_(std::make_unique<ListInputHandler>(this)),
    item_height_manager_(std::make_unique<ListItemHeightManager>()) {

}

}