#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/event/list_data_event_infos.h>
#include <zaf/creation.h>
#include <zaf/dynamic/object.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subject/subject.h>

namespace zaf {

/**
An interface that provides data for a ListControl.
*/
class ListDataSource : NonCopyableNonMovable {
public:
    /**
    Gets the empty ListDataSource instance.
    */
    static const std::shared_ptr<ListDataSource>& Empty();

public:
    ListDataSource() = default;
    virtual ~ListDataSource() = default;

    /**
    Gets the count of data.
    */
    virtual std::size_t GetDataCount() const {
        return 0;
    }

    /**
    Gets data at the specified index.
    */
    virtual std::shared_ptr<Object> GetDataAtIndex(std::size_t index) const {
        return Create<Object>();
    }

    /**
    Gets the data added event.

    @details
        This event is raised after data being added to the data source.
    */
    rx::Observable<ListDataAddedInfo> DataAddedEvent() const {
        return data_added_event_.AsObservable();
    }

    /**
    Gets the data removed event.

    @details
        This event is raised after data being removed from the data source.
    */
    rx::Observable<ListDataRemovedInfo> DataRemovedEvent() const {
        return data_removed_event_.AsObservable();
    }

    /**
    Gets the data updated event.

    @details
        This event is raised after data being updated.
    */
    rx::Observable<ListDataUpdatedInfo> DataUpdatedEvent() const {
        return data_updated_event_.AsObservable();
    }

    /**
    Gets the data moved event.

    @details
        This event is raised after data being moved to a new index.
    */
    rx::Observable<ListDataMovedInfo> DataMovedEvent() const {
        return data_moved_event_.AsObservable();
    }

protected:
    /**
    Raises the data added event.

    @param index
        The index where data is added.

    @param count
        The count of data is added.

    @details
        Derived classes should call this method to raise the data added event after adding data.
    */
    void NotifyDataAdded(std::size_t index, std::size_t count) const;

    /**
    Raises the data removed event.

    @param index
        The index of the first data removed.

    @param count
        The count of data removed.

    @details
        Derived classes should call this method to raise the data removed event after removing
        data.
    */
    void NotifyDataRemoved(std::size_t index, std::size_t count) const;

    /**
    Raises the data updated event.

    @param index
        The index of the first data updated.

    @param count
        The count of data updated.

    @details
        Derived classes should call this method to raise the data updated event after updating 
        data.
    */
    void NotifyDataUpdated(std::size_t index, std::size_t count) const;

    /**
    Raises the data moved event.

    @param previous_index
        The index of the data before moving.

    @param new_index
        The index of the data after moving.

    @details
        Derived classes should call this method to raise the data moved event after moving data.
    */
    void NotifyDataMoved(std::size_t previous_index, std::size_t new_index) const;

private:
    rx::Subject<ListDataAddedInfo> data_added_event_;
    rx::Subject<ListDataRemovedInfo> data_removed_event_;
    rx::Subject<ListDataUpdatedInfo> data_updated_event_;
    rx::Subject<ListDataMovedInfo> data_moved_event_;
};

}