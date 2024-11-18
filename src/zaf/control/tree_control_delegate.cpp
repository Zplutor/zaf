#include <zaf/control/tree_control_delegate.h>

namespace zaf {

const std::shared_ptr<TreeControlDelegate>& zaf::TreeControlDelegate::Default() {
    static auto default_delegate = std::make_shared<TreeControlDelegate>();
    return default_delegate;
}

}