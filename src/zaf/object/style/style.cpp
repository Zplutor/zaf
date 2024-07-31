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


void Style::AddSelector(std::shared_ptr<Selector> selector) {
    ZAF_EXPECT(selector);
    selectors_.emplace_back(std::move(selector));
}


void Style::ApplyTo(Object& object) {

    sheet_->ApplyTo(object);

    for (const auto& each_selector : selectors_) {
        each_selector->ApplyTo(object);
    }
}

}