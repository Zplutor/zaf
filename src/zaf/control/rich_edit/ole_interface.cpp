#include <zaf/control/rich_edit/ole_interface.h>
#include <zaf/base/error/com_error.h>

namespace zaf::rich_edit {

ObjectInfo OLEInterface::GetObjectInfoAt(std::size_t object_index) const {

    REOBJECT inner{};
    inner.cbStruct = sizeof(inner);

    HRESULT hresult = Inner()->GetObject(
        static_cast<LONG>(object_index),
        &inner,
        REO_GETOBJ_POLEOBJ);

    ZAF_THROW_IF_COM_ERROR(hresult);
    return ObjectInfo{ inner };
}


COMPtr<IOleObject> OLEInterface::GetObjectAt(std::size_t object_index) const {
    return GetObjectInfoAt(object_index).Object();
}


std::shared_ptr<EmbeddedObject> OLEInterface::GetEmbeddedObjectAt(std::size_t object_index) const {
    return EmbeddedObject::TryFromCOMPtr(GetObjectAt(object_index));
}

}