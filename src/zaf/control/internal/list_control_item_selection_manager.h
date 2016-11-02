#pragma once

#include <functional>
#include <vector>

namespace zaf {
namespace internal {

class ListControlItemSelectionManager {
public:
    void ReplaceSelection(std::size_t index, std::size_t count);
    void RevertSelection(std::size_t index);
    void AddSelection(std::size_t index, std::size_t count);
    void RemoveSelection(std::size_t index, std::size_t count);
    
    bool AdjustSelectionByAddingIndexes(std::size_t add_index, std::size_t add_count);
    bool AdjustSelectionByRemovingIndexes(std::size_t remove_index, std::size_t remove_count);

    std::size_t GetAllSelectedCount() const;
    std::vector<std::size_t> GetAllSelectedIndexes() const;
    std::size_t GetFirstSelectedIndex() const;
    bool IsIndexSelected(std::size_t index) const;

private:
    void AddSelectionRange(const std::pair<std::size_t, std::size_t>& range);

private:
    std::vector<std::pair<std::size_t, std::size_t>> selections_;
};

}
}