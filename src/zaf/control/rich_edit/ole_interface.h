#pragma once

#include <Richedit.h>
#include <richole.h>
#include <zaf/base/com_object.h>

namespace zaf::rich_edit {

class OLEInterface : public COMObject<IRichEditOle> {
public:
    /**
    Gets OLE object at specified index. 

    @param object_index
        Index of the object.

    @return
        The OLE object at specified index.

    @throw zaf::Error
        object_index is invalid, or other errors.
    */
    COMObject<IOleObject> GetObjectAt(std::size_t object_index) const;

    /**
    Gets total count of OLE objects in rich edit.
    */
    std::size_t GetObjectCount() const {
        return static_cast<std::size_t>(Inner()->GetObjectCount());
    }
};

}