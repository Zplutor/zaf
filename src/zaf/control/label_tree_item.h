#pragma once

#include <zaf/control/label.h>
#include <zaf/control/tree_item.h>

namespace zaf {

class LabelTreeItem : public TreeItem {
public:
    const std::shared_ptr<zaf::Label>& Label() const noexcept;

protected:
    void Initialize() override;
    void Layout(const zaf::Rect&) override;

    void OnItemDataChanged() override;

private:
    std::shared_ptr<zaf::Label> label_;
};

}
