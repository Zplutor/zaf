#include <zaf/control/internal/tree_control/tree_selection_manager.h>
#include <zaf/base/container/utility/append.h>
#include <zaf/base/container/utility/contain.h>

namespace zaf::internal {

bool TreeSelectionManager::IsIndexPathSelected(const IndexPath& index_path) {
    return Contain(selected_index_paths_, index_path);
}


void TreeSelectionManager::AddSelectedIndexPaths(const std::vector<IndexPath>& index_paths) {
    Append(selected_index_paths_, index_paths);
}


void TreeSelectionManager::RemoveSelectedIndexPaths(const std::vector<IndexPath>& index_paths) {

    for (const auto& each_index_path : index_paths) {
        selected_index_paths_.erase(each_index_path);
    }
}


void TreeSelectionManager::ReplaceSelectedIndexPahts(const std::vector<IndexPath>& index_paths) {

    selected_index_paths_.clear();
    Append(selected_index_paths_, index_paths);
}

}