#pragma once

#include <Richedit.h>
#include <richole.h>
#include <zaf/base/com_ptr.h>

namespace zaf::rich_edit {

class ObjectInfo {
public:
    ObjectInfo();
    explicit ObjectInfo(const REOBJECT& inner);

    std::size_t Index() const {
        return static_cast<std::size_t>(inner_.cp);
    }

    COMPtr<IOleObject> Object() const {
        return object_;
    }

    const REOBJECT& Inner() const {
        return inner_;
    }

private:
    REOBJECT inner_{};
    COMPtr<IOleObject> object_;
    COMPtr<IOleClientSite> client_site_;
    COMPtr<IStorage> storage_;
};

}