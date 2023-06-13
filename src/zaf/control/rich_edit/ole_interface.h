#pragma once

#include <Richedit.h>
#include <richole.h>
#include <zaf/base/com_ptr.h>
#include <zaf/control/rich_edit/embedded_object.h>
#include <zaf/control/rich_edit/object_info.h>

namespace zaf::rich_edit {

class OLEInterface : public COMPtr<IRichEditOle> {
public:
    /**
    Gets total count of OLE objects in rich edit.
    */
    std::size_t GetObjectCount() const noexcept {
        return static_cast<std::size_t>(Inner()->GetObjectCount());
    }

    /**
    Gets OLE object info at specified index.

    @param object_index
        Index of the object.

    @return 
        Information of the OLE object at specified index.

    @throw zaf::Error
        object_index is invalid, or other errors.
    */
    ObjectInfo GetObjectInfoAt(std::size_t object_index) const;

    /**
    Gets OLE object at specified index. 

    @param object_index
        Index of the object.

    @return
        The OLE object at specified index.

    @throw zaf::Error
        object_index is invalid, or other errors.
    */
    COMPtr<IOleObject> GetObjectAt(std::size_t object_index) const;

    /**
    Gets OLE object at specified index and transform it to EmbeddedObject, if can.

    @param object_index
        Index of the object.

    @return
        Returns a pointer to EmbeddedObject. Returns nullptr if the OLE object is not an 
        EmbeddedObject.

    @throw zaf::Error
        object_index is invalid, or other errors.
    */
    std::shared_ptr<EmbeddedObject> GetEmbeddedObjectAt(std::size_t object_index) const;
};

}