#include <zaf/control/internal/list_control/list_control_item_selection_manager.h>

namespace zaf {
namespace internal {

std::size_t ListControlItemSelectionManager::GetAllSelectedCount() const {

    std::size_t count = 0;
    for (const auto& each_range : range_set_) {
        count += each_range.length;
    }
    return count;
}


std::vector<std::size_t> ListControlItemSelectionManager::GetAllSelectedIndexes() const {

    std::vector<std::size_t> indexes;
    for (const auto& each_range : range_set_) {
        for (auto index : each_range) {
            indexes.push_back(index);
        }
    }
    return indexes;
}


}
}