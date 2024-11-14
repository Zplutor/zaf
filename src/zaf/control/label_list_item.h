#pragma once

#include <zaf/control/label.h>
#include <zaf/control/list_item.h>

namespace zaf {

class LabelListItem : public ListItem {
public:
    ZAF_OBJECT;

    const std::shared_ptr<Label>& Label() const noexcept;

protected:
    void Initialize() override;
    void Layout(const zaf::Rect& old_rect) override;
    void OnItemDataChanged() override;

private:
    std::shared_ptr<zaf::Label> label_;
};

ZAF_OBJECT_BEGIN(LabelListItem);
ZAF_OBJECT_END;

}
