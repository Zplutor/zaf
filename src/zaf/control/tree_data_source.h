#pragma once

#include <zaf/base/define.h>
#include <zaf/object/object.h>
#include <zaf/rx/subject.h>

namespace zaf {

class TreeDataSourceDataAddInfo;
class TreeDataSourceDataRemoveInfo;
class TreeDataSourceDataUpdateInfo;

class TreeDataSource {
public:
    virtual bool DoesDataHasChildren(const std::shared_ptr<Object>& data) {
        return false;
    }

    virtual std::size_t GetChildDataCount(const std::shared_ptr<Object>& parent_data) {
        return 0;
    }

    virtual std::shared_ptr<Object> GetChildDataAtIndex(
        const std::shared_ptr<Object>& parent_data,
        std::size_t index) {

        return Object::Dumb();
    }

    Observable<TreeDataSourceDataAddInfo> DataAddEvent() {
        return data_add_event_.GetObservable();
    }

    Observable<TreeDataSourceDataRemoveInfo> DataRemoveEvent() {
        return data_remove_event_.GetObservable();
    }

    Observable<TreeDataSourceDataUpdateInfo> DataUpdateEvent() {
        return data_update_event_.GetObservable();
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