#include <zaf/control/internal/tree_control/tree_selection_manager.h>
#include <zaf/base/container/utility/append.h>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/conversion.h>
#include <zaf/base/container/utility/map.h>

namespace zaf::internal {

bool TreeSelectionManager::IsIndexPathSelected(const IndexPath& index_path) const {
    return Contain(selected_nodes_, index_path);
}


std::vector<std::shared_ptr<Object>> TreeSelectionManager::GetAllSelectedNodeData() const {
    return GetValues(selected_nodes_);
}


std::shared_ptr<Object> TreeSelectionManager::GetFirstSelectedNodeData() const {

    if (!selected_nodes_.empty()) {
        return selected_nodes_.begin()->second;
    }
    return nullptr;
}


void TreeSelectionManager::AddSelectedNodes(
    const std::map<IndexPath, std::shared_ptr<Object>>& nodes) {

    zaf::Append(selected_nodes_, nodes);
}


void TreeSelectionManager::RemoveSelectedNodes(const std::vector<IndexPath>& index_paths) {

    for (const auto& each_index_path : index_paths) {
        selected_nodes_.erase(each_index_path);
    }
}


void TreeSelectionManager::Clear() {
    selected_nodes_.clear();
}

}