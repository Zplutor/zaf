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

    virtual void OnItemDataChanged(const std::shared_ptr<Object>& previous_data);

    virtual void RecoverFocus();

private:
    friend class internal::ListVisibleItemManager;

    void SetItemData(std::shared_ptr<Object> data);

private:
    bool IsInFocusContext() const;

private:
    std::shared_ptr<Object> item_data_;
};

}