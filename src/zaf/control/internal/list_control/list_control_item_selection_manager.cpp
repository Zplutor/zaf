#include <zaf/control/internal/list_control/list_control_item_selection_manager.h>

namespace zaf {
namespace internal {

std::size_t ListControlItemSelectionManager::GetAllSelectedCount() const {

    std::size_t count = 0;

    for (std::size_t index = 0; index < range_manager_.RangeCount(); ++index) {
        count += range_manager_.GetRange(index).length;
    }

    return count;
}


std::vector<std::size_t> ListControlItemSelectionManager::GetAllSelectedIndexes() const {

    std::vector<std::size_t> indexes;

    for (std::size_t index = 0; index < range_manager_.RangeCount(); ++index) {

        auto range = range_manager_.GetRange(index);

        for (std::size_t index = range.index;
             index < range.index + range.length;
             ++index) {

            indexes.push_back(index);
        }
    }

    return indexes;
}


}
}