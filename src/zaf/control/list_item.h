#pragma once

#include <zaf/control/textual_control.h>

namespace zaf::internal {
class ListVisibleItemManager;
}

namespace zaf {

class ListItem : public TextualControl {
public:
    const std::shared_ptr<Object>& ItemData() const {
        return item_data_;
    }

protected:
    void Initialize() override;

    void OnFocusGained(const FocusGainedInfo& event_info) override;
    void OnFocusLost(const FocusLostInfo& event_info) override;

    virtual void RecoverFocus();

private:
    friend class internal::ListVisibleItemManager;

    void SetItemData(const std::shared_ptr<Object>& data) {
        ZAF_EXPECT(data);
        item_data_ = data;
    }

private:
    bool IsInFocusContext() const;

private:
    std::shared_ptr<Object> item_data_;
};

}