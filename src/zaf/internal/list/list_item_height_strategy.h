#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>

namespace zaf::internal {

class ListItemHeightStrategy : NonCopyableNonMovable {
public:
    ListItemHeightStrategy(
        std::weak_ptr<ListDataSource> data_source,
        std::weak_ptr<ListControlDelegate> delegate);

    virtual ~ListItemHeightStrategy() = default;

    virtual void Initialize();

    virtual std::pair<float, float> GetItemPositionAndHeight(std::size_t index) = 0;

    virtual std::optional<std::size_t> GetItemIndex(float position) = 0;

    virtual std::pair<std::size_t, std::size_t> GetItemRange(
        float begin_position,
        float end_position) = 0;

    virtual float GetTotalHeight() = 0;

    virtual void OnDataAdded(const ListDataAddedInfo& event_info);
    virtual void OnDataUpdated(const ListDataUpdatedInfo& event_info);
    virtual void OnDataMoved(const ListDataMovedInfo& event_info);
    virtual void OnDataRemoved(const ListDataRemovedInfo& event_info);

protected:
    std::shared_ptr<ListDataSource> DataSource() const {
        return data_source_.lock();
    }

    std::shared_ptr<ListControlDelegate> Delegate() const {
        return delegate_.lock();
    }

private:
    std::weak_ptr<ListDataSource> data_source_;
    std::weak_ptr<ListControlDelegate> delegate_;
};

}