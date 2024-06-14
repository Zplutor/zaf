#pragma once

#include <map>
#include <string>
#include <zaf/object/object.h>
#include <zaf/object/style/sheet.h>

namespace zaf {

class Style : public Object, NonCopyableNonMovable {
public:
    ZAF_DECLARE_TYPE;

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

}