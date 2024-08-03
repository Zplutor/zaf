#include <zaf/control/style/style_collection.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/control/control.h>

namespace zaf {

StyleCollection::StyleCollection(Control* owner) : owner_(owner) {
    ZAF_EXPECT(owner_);
}


void StyleCollection::Add(std::shared_ptr<Style> style) {
    styles_.push_back(std::move(style));
    owner_->NeedRepaint();
}


void StyleCollection::Clear() {
    styles_.clear();
    owner_->NeedRepaint();
}

}