#include <zaf/control/internal/tree_control/tree_data.h>
#include <zaf/base/container/utility/map.h>
#include <zaf/base/container/utility/range.h>

namespace zaf::internal {

void TreeData::AddChildren(const IndexPath& parent, std::size_t count) {
    node_child_count_map_[parent] += count;
    //TODO: Update children's index path.
}


void TreeData::RemoveChildren(const IndexPath& parent, std::size_t count) {

    auto iterator = node_child_count_map_.find(parent);
    if (iterator == node_child_count_map_.end()) {
        return;
    }

    if (iterator->second > count) {
        iterator->second -= count;

        //TODO: Update children's index path.
    }
    else {
        node_child_count_map_.erase(iterator);   
    }
}


std::size_t TreeData::GetNodeCount() const {

    std::size_t result;
    for (auto each_count : Values(node_child_count_map_)) {
        result += each_count;
    }
    return result;
}


IndexPath TreeData::GetIndexPathAtIndex(std::size_t index) {

    struct LevelInfo {
        std::size_t children_count{};
        std::size_t last_handled_child_index{};
    };

    std::vector<std::size_t> root_path;
    std::vector<LevelInfo> level_infos;
    std::size_t current_global_index{};

    auto iterator = node_child_count_map_.begin();
    while (iterator != node_child_count_map_.end()) {

        const auto& current_node_path = iterator->first;
        if (current_node_path.empty()) {

            LevelInfo root_level_infos;
            root_level_infos.children_count = iterator->second;
            root_level_infos.last_handled_child_index = 0;

            level_infos.push_back(root_level_infos);
            ++iterator;
            continue;
        }

        //Calculate current global index
        std::size_t current_node_index = 
            current_global_index + 
            (current_node_path.back() - level_infos.back().last_handled_child_index);

        //Return current node path if it is the expected one.
        if (current_node_index == index) {
            return current_node_path;
        }

        if (current_node_index >= index) {
            auto revised_node_path = current_node_path;
            revised_node_path.back() -= current_node_index - index;
            return revised_node_path;
        }

        auto next_iterator = std::next(iterator);

        const auto& next_node_path =
            next_iterator != node_child_count_map_.end() ?
            next_iterator->first : 
            root_path;

        //Next expanded node is child of current node.
        if (next_node_path.size() > current_node_path.size()) {

            level_infos.back().last_handled_child_index = current_node_path.back();

            //Enter child node
            LevelInfo level_info;
            level_info.children_count = iterator->second;
            level_info.last_handled_child_index = 0;
            level_infos.push_back(level_info);

            iterator = next_iterator;

            //Global index should increase 1 when entering next level.
            current_global_index = current_node_index + 1;
            continue;
        }

        //Next expanded node is not child of current node, 
        //check children of current node.
        //Increase 1 to global index implicitly.
        auto last_child_global_index = current_node_index + iterator->second;
        if (last_child_global_index >= index) {
            auto child_path = current_node_path;
            child_path.push_back(index - current_node_index - 1);
            return child_path;
        }

        //Finish checking children, return to current level.
        //Global index should decrease 1 when returning, in order to balance the increasement 
        //when entering the child level
        current_global_index = (last_child_global_index - 1) + 1;

        level_infos.back().last_handled_child_index = current_node_path.back();

        if (next_node_path.size() < current_node_path.size()) {

            auto backward_node_path = current_node_path;
            while (backward_node_path.size() > next_node_path.size()) {

                std::size_t last_child_index = 
                    (level_infos.back().children_count - backward_node_path.back()) - 1 + 
                    current_global_index;

                //Check remain children in current level.
                if (last_child_index >= index) {
                    auto revised_node_path = backward_node_path;
                    revised_node_path.back() += index - current_global_index;
                    return revised_node_path;
                }

                //Return to previous level.
                current_global_index = (last_child_index - 1) + 1;

                backward_node_path.pop_back();
                level_infos.pop_back();
            }
        }

        //Next expanded node is sibling, enter next expaned node
        iterator = next_iterator;
    }

    return {};
}

}