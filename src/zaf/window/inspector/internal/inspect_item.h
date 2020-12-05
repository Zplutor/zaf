#pragma once

#include <zaf/control/tree_item.h>

namespace zaf::internal {

class InspectItem : public TreeItem {
public:
    void SetIsHighlight(bool is_highlight);

protected:
    void Initialize() override;

private:
    bool is_highlight_{};
};

}