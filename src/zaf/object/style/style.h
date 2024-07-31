#pragma once

#include <map>
#include <string>
#include <zaf/object/object.h>
#include <zaf/object/style/selector.h>
#include <zaf/object/style/sheet.h>
#include <zaf/object/style/style_parser.h>

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

    void AddSelector(std::shared_ptr<Selector> selector);

    void ApplyTo(Object& object);

private:
    std::shared_ptr<zaf::Sheet> sheet_;
    std::vector<std::shared_ptr<Selector>> selectors_;
};

ZAF_OBJECT_BEGIN(Style);
ZAF_OBJECT_PARSER(StyleParser);
ZAF_OBJECT_END;

}