#pragma once

#include <zaf/creation.h>
#include <zaf/object/object.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subject.h>

namespace zaf {

class Object;
class ListDataSourceDataAddInfo;
class ListDataSourceDataRemoveInfo;
class ListDataSourceDataUpdateInfo;

class ListDataSource {
public:
    ListDataSource() = default;
    virtual ~ListDataSource() = default;

    ListDataSource(const ListDataSource&) = delete;
    ListDataSource& operator=(const ListDataSource&) = delete;

    virtual std::size_t GetDataCount() {
        return 0;
    }

    virtual std::shared_ptr<Object> GetDataAtIndex(std::size_t index) {
        return Create<Object>();
    }

    /**
     Get item add event.
     */
    Observable<ListDataSourceDataAddInfo> DataAddEvent() {
        return data_add_event_.AsObservable();
    }

    /**
     Get item remove event.
     */
    Observable<ListDataSourceDataRemoveInfo> DataRemoveEvent() {
        return data_remove_event_.AsObservable();
    }

    /**
     Get item update event.
     */
    Observable<ListDataSourceDataUpdateInfo> DataUpdateEvent() {
        return data_update_event_.AsObservable();
    }

protected:
    /**
     Raise an item add event.

     @param index
         The index where items are added.

     @param count
         The count of items added.

     You must call this method to update the list control after adding
     items to data source.
     */
    void NotifyDataAdd(std::size_t index, std::size_t count);

    /**
     Raise an item remove event.

     @param index
         The index of the first item removed.

     @param count
         The count of items removed.

     You must call this method to update the list control after removing
     items from data source.
     */
    void NotifyDataRemove(std::size_t index, std::size_t count);

    /**
     Raise an item update event.

     @param index
         The index of the first item updated.

     @param count
         The count of items updated.

     You must call this method to update the list control after updating
     items in data source.
     */
    void NotifyDataUpdate(std::size_t index, std::size_t count);

private:
    Subject<ListDataSourceDataAddInfo> data_add_event_;
    Subject<ListDataSourceDataRemoveInfo> data_remove_event_;
    Subject<ListDataSourceDataUpdateInfo> data_update_event_;
};


class ListDataSourceDataAddInfo {
public:
    std::size_t index{};
    std::size_t count{};
};


class ListDataSourceDataRemoveInfo {
public:
    std::size_t index{};
    std::size_t count{};
};


class ListDataSourceDataUpdateInfo {
public:
    std::size_t index{};
    std::size_t count{};
};

}