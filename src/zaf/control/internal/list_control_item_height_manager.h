#pragma once

#include <memory>
#include <vector>
#include <zaf/control/list_control.h>

namespace zaf {
namespace internal {

class ListControlItemHeightManager {
public:
    ListControlItemHeightManager(const std::shared_ptr<ListControl::ItemSource>& item_source);
    ~ListControlItemHeightManager();

    void LoadItemHeights();

    std::pair<float, float> GetItemPositionAndHeight(std::size_t index) const;
    std::pair<std::size_t, std::size_t> GetItemIndexAndCount(float begin_position, float end_position) const;

    float GetTotalHeight() const;

    std::size_t GetItemCount() const {
        return item_count_;
    }

private:
    void ItemAdd(ListControl::ItemSource& item_source, std::size_t index, std::size_t count);
    void ItemRemove(ListControl::ItemSource& item_source, std::size_t index, std::size_t count);
    void ItemUpdate(ListControl::ItemSource& item_source, std::size_t index, std::size_t count);

private:
    std::shared_ptr<ListControl::ItemSource> item_source_;
    std::size_t item_count_;
    bool has_variable_heights_;
    float item_height_;
    std::vector<float> item_positions_;
};

}
}