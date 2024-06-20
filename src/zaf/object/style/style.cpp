#include <zaf/object/style/style.h>
#include <zaf/base/error/contract_error.h>

namespace zaf {

ZAF_OBJECT_IMPL(Style);


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