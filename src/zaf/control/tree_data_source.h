#pragma once

#include <zaf/base/define.h>
#include <zaf/base/non_copyable.h>
#include <zaf/creation.h>
#include <zaf/object/object.h>
#include <zaf/rx/subject.h>

namespace zaf {

class TreeDataSourceDataAddInfo;
class TreeDataSourceDataRemoveInfo;
class TreeDataSourceDataUpdateInfo;

class TreeDataSource : NonCopyableNonMovable {
public:
    /**
    Gets the empty TreeDataSource instance.
    */
    static const std::shared_ptr<TreeDataSource>& Empty();

public:
    TreeDataSource() = default;
    virtual ~TreeDataSource() = default;

    virtual bool DoesDataHasChildren(const std::shared_ptr<Object>& data) {
        return false;
    }

    virtual std::size_t GetChildDataCount(const std::shared_ptr<Object>& parent_data) {
        return 0;
    }

    virtual std::shared_ptr<Object> GetChildDataAtIndex(
        const std::shared_ptr<Object>& parent_data,
        std::size_t index) {

        return Create<Object>();
    }

    Observable<TreeDataSourceDataAddInfo> DataAddEvent() {
        return data_add_event_.AsObservable();
    }

    Observable<TreeDataSourceDataRemoveInfo> DataRemoveEvent() {
        return data_remove_event_.AsObservable();
    }

    Observable<TreeDataSourceDataUpdateInfo> DataUpdateEvent() {
        return data_update_event_.AsObservable();
    }

protected:
    void NotifyDataAdd(
        const std::shared_ptr<Object>& parent_data, 
        std::size_t index, 
        std::size_t count);

    void NotifyDataRemove(
        const std::shared_ptr<Object>& parent_data,
        std::size_t index,
        std::size_t count);

    void NotifyDataUpdate(
        const std::shared_ptr<Object>& parent_data,
        std::size_t index,
        std::size_t count);

private:
    Subject<TreeDataSourceDataAddInfo> data_add_event_;
    Subject<TreeDataSourceDataRemoveInfo> data_remove_event_;
    Subject<TreeDataSourceDataUpdateInfo> data_update_event_;
};


class TreeDataSourceDataAddInfo {
public:
    std::shared_ptr<Object> parent_data;
    std::size_t index{};
    std::size_t count{};
};


class TreeDataSourceDataRemoveInfo {
public:
    std::shared_ptr<Object> parent_data;
    std::size_t index{};
    std::size_t count{};
};


class TreeDataSourceDataUpdateInfo {
public:
    std::shared_ptr<Object> parent_data;
    std::size_t index{};
    std::size_t count{};
};

}