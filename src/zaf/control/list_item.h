#pragma once

#include <zaf/control/control.h>

namespace zaf::internal {
class ListVisibleItemManager;
}

namespace zaf {

class ListItem : public Control {
public:
    ZAF_OBJECT;

    const std::shared_ptr<dynamic::Object>& ItemData() const {
        return item_data_;
    }

protected:
    void Initialize() override;

    void OnFocusGained(const FocusGainedInfo& event_info) override;
    void OnFocusLost(const FocusLostInfo & event_info) override;

    virtual void OnItemDataChanged();

private:
    friend class internal::ListVisibleItemManager;

    void SetItemData(std::shared_ptr<dynamic::Object> data);

private:
    bool IsInFocusContext() const;
    void RepaintOnFocusChanged();

private:
    std::shared_ptr<dynamic::Object> item_data_;
};

ZAF_OBJECT_BEGIN(ListItem);
ZAF_OBJECT_END;

}