#include <zaf/control/rich_edit/object_info.h>

namespace zaf::rich_edit {

ObjectInfo::ObjectInfo() {
    inner_.cbStruct = sizeof(inner_);
}


ObjectInfo::ObjectInfo(const REOBJECT& inner) : inner_(inner) {

    inner_.cbStruct = sizeof(inner_);

    object_.Reset(inner_.poleobj);
    client_site_.Reset(inner_.polesite);
    storage_.Reset(inner_.pstg);
}

}