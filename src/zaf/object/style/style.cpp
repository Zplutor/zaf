#include <zaf/object/style/style.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(Style);
ZAF_DEFINE_TYPE_END;

ZAF_DEFINE_PROPERTY(Style);


Style::Style() : sheet_(std::make_shared<zaf::Sheet>()) {

}


void Style::SetSheet(std::shared_ptr<zaf::Sheet> sheet) {
    ZAF_EXPECT(sheet);
    sheet_ = std::move(sheet);
}


void Style::Apply(Object& object) {
    sheet_->Apply(object);
}

}