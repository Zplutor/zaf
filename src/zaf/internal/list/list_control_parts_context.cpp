#include <zaf/internal/list/list_control_parts_context.h>

namespace zaf::internal {

ListControlPartsContext::ListControlPartsContext(ScrollBox* owner) :
    owner_(owner),
    core_(std::make_unique<ListControlCore>(this)),
    input_handler_(std::make_unique<ListInputHandler>(this)),
    item_height_manager_(std::make_unique<ListItemHeightManager>()),
    selection_manager_(std::make_unique<ListSelectionManager>(this)),
    selection_store_(std::make_unique<ListSelectionStore>()),
    visible_item_manager_(std::make_unique<ListVisibleItemManager>(this)),
    focus_store_(std::make_unique<ListFocusStore>(this)) {

}

}