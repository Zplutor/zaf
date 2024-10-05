#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/event/list_data_event_infos.h>
#include <zaf/creation.h>
#include <zaf/object/object.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subject.h>

namespace zaf {

class ListDataSource : NonCopyableNonMovable {
public:
    ListDataSource() = default;
    virtual ~ListDataSource() = default;

    virtual std::size_t GetDataCount() {
        return 0;
    }

    virtual std::shared_ptr<Object> GetDataAtIndex(std::size_t index) {
        return Create<Object>();
    }

    /**
     Get item add event.
     */
    Observable<ListDataAddedInfo> DataAddedEvent() const {
        return data_added_event_.AsObservable();
    }

    /**
     Get item remove event.
     */
    Observable<ListDataRemovedInfo> DataRemovedEvent() const {
        return data_removed_event_.AsObservable();
    }

    /**
     Get item update event.
     */
    Observable<ListDataUpdatedInfo> DataUpdatedEvent() const {
        return data_updated_event_.AsObservable();
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
    void NotifyDataAdded(std::size_t index, std::size_t count);

    /**
     Raise an item remove event.

     @param index
         The index of the first item removed.

     @param count
         The count of items removed.

     You must call this method to update the list control after removing
     items from data source.
     */
    void NotifyDataRemoved(std::size_t index, std::size_t count);

    /**
     Raise an item update event.

     @param index
         The index of the first item updated.

     @param count
         The count of items updated.

     You must call this method to update the list control after updating
     items in data source.
     */
    void NotifyDataUpdated(std::size_t index, std::size_t count);

private:
    Subject<ListDataAddedInfo> data_added_event_;
    Subject<ListDataRemovedInfo> data_removed_event_;
    Subject<ListDataUpdatedInfo> data_updated_event_;
};

}