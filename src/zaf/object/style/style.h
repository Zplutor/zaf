#pragma once

#include <map>
#include <string>
#include <zaf/object/object.h>
#include <zaf/object/property_macros.h>
#include <zaf/object/style/sheet.h>

namespace zaf {

class Style : public Object, NonCopyableNonMovable {
public:
    ZAF_DECLARE_TYPE;
    ZAF_DECLARE_PROPERTY;

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

ZAF_PROPERTY_BEGIN(Style);
ZAF_PROPERTY(Sheet);
ZAF_PROPERTY_END;

}