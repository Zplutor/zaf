#include <zaf/control/internal/tree_control/tree_expand_manager.h>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/error/check.h>
#include <zaf/control/internal/tree_control/utility.h>

namespace zaf::internal {

std::size_t TreeExpandManager::Expand(
    const IndexPath& index_path,
    const ExpandNodeInfo& expand_node_info,
    TreeData& tree_data) {

    //Save expanded node info.
    auto insert_result = expanded_nodes_.insert(std::make_pair(index_path, expand_node_info));
    ZAF_CHECK(insert_result.second);

    tree_data.AddChildren(index_path, 0, expand_node_info.children_count);
    std::size_t total_children_count = expand_node_info.children_count;

    //Insert children to UI tree data.
    IndexPath parent_index_path = index_path;
    for (auto iterator = std::next(insert_result.first); 
         iterator != expanded_nodes_.end();
         ++iterator) {

        const auto& current_index_path = iterator->first;
        if (!IsAncestorOf(index_path, current_index_path)) {
            break;
        }

        int path_length_difference = 
            static_cast<int>(current_index_path.size()) - 
            static_cast<int>(parent_index_path.size());

        //Ignore current node if it is not a direct child of parent node.
        if (path_length_difference > 1) {
            continue;
        }

        while (path_length_difference < 1) {
            parent_index_path.pop_back();
            ++path_length_difference;
        }

        //Ignore current node if its parent is not in tree.
        if (!IsAncestorOf(parent_index_path, current_index_path)) {
            continue;
        }

        tree_data.AddChildren(current_index_path, 0, iterator->second.children_count);
        total_children_count += iterator->second.children_count;

        parent_index_path.push_back(current_index_path.back());        
    }

    return total_children_count;
}


void TreeExpandManager::Collapse(const IndexPath& index_path) {
    expanded_nodes_.erase(index_path);
}


void TreeExpandManager::Clear() {
    expanded_nodes_.clear();
}


bool TreeExpandManager::IsNodeExpanded(const IndexPath& index_path) const {
    return Contain(expanded_nodes_, index_path);
}


const ExpandNodeInfo* TreeExpandManager::GetNodeInfo(const IndexPath& index_path) const {
    return Find(expanded_nodes_, index_path);
}


const IndexPath* TreeExpandManager::GetNodeIndexPath(const std::shared_ptr<Object>& data) const {

    for (const auto& each_pair : expanded_nodes_) {

        const auto& node_data = each_pair.second.node_data;

        bool is_found{};
        if (!node_data) {
            if (!data) {
                is_found = true;
            }
        }
        else if (node_data->IsEqual(*data)) {
            is_found = true;
        }

        if (is_found) {
            return &each_pair.first;
        }
    }

    return nullptr;
}


void TreeExpandManager::AddChildren(const IndexPath& index_path, std::size_t count) {

    auto node_info = Find(expanded_nodes_, index_path);
    if (node_info) {
        node_info->children_count += count;
    }
}

}