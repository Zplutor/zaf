#pragma once

#include <map>
#include <string>
#include <zaf/object/object.h>
#include <zaf/object/property_support.h>
#include <zaf/object/style/sheet.h>

namespace zaf {

class Style : public Object, NonCopyableNonMovable {
public:
    ZAF_OBJECT;

public:
    Style();

    const std::shared_ptr<zaf::Sheet>& Sheet() const {
        return sheet_;
    }

    void SetSheet(std::shared_ptr<zaf::Sheet> sheet);

    void Apply(Object& object);

private:
    std::shared_ptr<zaf::Sheet> sheet_;
};

ZAF_OBJECT_BEGIN(Style);
ZAF_OBJECT_PROPERTY(Sheet);
ZAF_OBJECT_END;

}