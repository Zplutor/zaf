#pragma once

#include <memory>
#include <vector>

namespace zaf {

class ListControlDelegate;
class ListDataSource;

namespace internal {

class ListControlItemHeightManager {
public:
    ListControlItemHeightManager(
        const std::shared_ptr<ListDataSource>& data_source,
        const std::shared_ptr<ListControlDelegate>& delegate);

    ~ListControlItemHeightManager();

    void ResetDelegate(const std::shared_ptr<ListControlDelegate>& delegate);

    void ReloadItemHeights();

    std::pair<float, float> GetItemPositionAndHeight(std::size_t index) const;
    std::pair<std::size_t, std::size_t> GetItemIndexAndCount(
        float begin_position, 
        float end_position) const;

    float GetTotalHeight() const;

    std::size_t GetItemCount() const {
        return item_count_;
    }

private:
    void RegisterDataSourceEvents();
    void UnregisterDataSourceEvents();

    void ItemAdd(std::size_t index, std::size_t count);
    void ItemRemove(std::size_t index, std::size_t count);
    void ItemUpdate(std::size_t index, std::size_t count);

private:
    std::weak_ptr<ListDataSource> data_source_{};
    std::weak_ptr<ListControlDelegate> delegate_{};
    std::size_t item_count_{};
    bool has_variable_heights_{};
    float item_height_{};
    std::vector<float> item_positions_;
};

}
}