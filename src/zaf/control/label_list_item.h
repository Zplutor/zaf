#pragma once

#include <zaf/control/label.h>
#include <zaf/control/list_item.h>

namespace zaf {

class LabelListItem : public ListItem {
public:
    ZAF_OBJECT;

    const std::shared_ptr<zaf::Label>& Label() const noexcept;
    void SetLabel(std::shared_ptr<zaf::Label> label);

protected:
    void Initialize() override;
    void Layout(const zaf::Rect& old_rect) override;
    void OnItemDataChanged() override;

private:
    void InstallLabel(std::shared_ptr<zaf::Label> label);

private:
    std::shared_ptr<zaf::Label> label_;
};

ZAF_OBJECT_BEGIN(LabelListItem);
ZAF_OBJECT_PROPERTY(Label);
ZAF_OBJECT_END;

}
