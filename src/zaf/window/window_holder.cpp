#include <zaf/window/window_holder.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/window/window.h>

namespace zaf {

WindowHolder::WindowHolder(const std::shared_ptr<zaf::Window>& window) :
    window_(window) {

    ZAF_EXPECT(window_);
}


WindowHolder::~WindowHolder() {

    if (window_) {
        window_->Destroy();
    }
}

}