#pragma once

#include <Richedit.h>
#include <richole.h>
#include <zaf/base/com_object.h>

namespace zaf::rich_edit {

class ObjectInfo {
public:
    ObjectInfo();
    explicit ObjectInfo(const REOBJECT& inner);

    std::size_t Index() const {
        return static_cast<std::size_t>(inner_.cp);
    }

    COMObject<IOleObject> Object() const {
        return object_;
    }

    const REOBJECT& Inner() const {
        return inner_;
    }

private:
    REOBJECT inner_{};
    COMObject<IOleObject> object_;
    COMObject<IOleClientSite> client_site_;
    COMObject<IStorage> storage_;
};

}