#pragma once

#include <zaf/base/event.h>
#include <zaf/object/object.h>

namespace zaf {

class Object;

class ListDataSource {
public:
    /**
     Type of item add event.
    */
    typedef Event<std::size_t, std::size_t> DataAddEvent;

    /**
     Type of item remove event.
     */
    typedef Event<std::size_t, std::size_t> DataRemoveEvent;

    /**
     Type of item update event.
     */
    typedef Event<std::size_t, std::size_t> DataUpdateEvent;

public:
    ListDataSource() = default;
    virtual ~ListDataSource() = default;

    ListDataSource(const ListDataSource&) = delete;
    ListDataSource& operator=(const ListDataSource&) = delete;

    virtual std::size_t GetDataCount() {
        return 0;
    }

    virtual std::shared_ptr<Object> GetDataAtIndex(std::size_t index) {
        return Object::Empty();
    }

    /**
     Get item add event.
     */
    DataAddEvent::Proxy GetDataAddEvent() {
        return DataAddEvent::Proxy(data_add_event_);
    }

    /**
     Get item remove event.
     */
    DataRemoveEvent::Proxy GetDataRemoveEvent() {
        return DataRemoveEvent::Proxy(data_remove_event_);
    }

    /**
     Get item update event.
     */
    DataRemoveEvent::Proxy GetDataUpdateEvent() {
        return DataUpdateEvent::Proxy(data_update_event_);
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
    DataAddEvent data_add_event_;
    DataRemoveEvent data_remove_event_;
    DataUpdateEvent data_update_event_;
};

}