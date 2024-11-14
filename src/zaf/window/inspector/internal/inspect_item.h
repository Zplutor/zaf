#pragma once

#include <zaf/control/label_tree_item.h>

namespace zaf::internal {

class InspectItem : public LabelTreeItem {
public:
    void SetIsHighlight(bool is_highlight);

protected:
    void Initialize() override;
    void OnItemDataChanged() override;

private:
    std::wstring GetItemText() const;

private:
    bool is_highlight_{};
};

}