#include <zaf/control/rich_edit/ole_interface.h>
#include <zaf/base/error/com_error.h>

namespace zaf::rich_edit {

COMObject<IOleObject> OLEInterface::GetObjectAt(std::size_t object_index) const {

    REOBJECT object_info{};
    object_info.cbStruct = sizeof(object_info);

    HRESULT hresult = Inner()->GetObject(
        static_cast<LONG>(object_index),
        &object_info,
        REO_GETOBJ_POLEOBJ);

    ZAF_THROW_IF_COM_ERROR(hresult);

    return COMObject<IOleObject>{ object_info.poleobj };
}

}