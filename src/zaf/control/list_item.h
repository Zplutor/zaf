#pragma once

#include <zaf/control/control.h>

namespace zaf::internal {
class ListVisibleItemManager;
}

namespace zaf {

class ListItem : public Control {
public:
    ZAF_OBJECT;

    const std::shared_ptr<Object>& ItemData() const {
        return item_data_;
    }

protected:
    void Initialize() override;

    virtual void OnItemDataChanged();

private:
    friend class internal::ListVisibleItemManager;

    void SetItemData(std::shared_ptr<Object> data);

private:
    bool IsInFocusContext() const;

private:
    std::shared_ptr<Object> item_data_;
};

ZAF_OBJECT_BEGIN(ListItem);
ZAF_OBJECT_END;

}