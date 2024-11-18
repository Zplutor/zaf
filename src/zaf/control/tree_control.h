#pragma once

#include <zaf/control/scroll_box.h>
#include <zaf/control/list_selection_mode.h>
#include <zaf/rx/observable.h>

namespace zaf {
namespace internal {
class TreeCore;
}

class TreeControlDelegate;
class TreeControlItemCollapseInfo;
class TreeControlItemExpandInfo;
class TreeControlSelectionChangeInfo;
class TreeDataSource;
class TreeItemContainer;

class TreeControl : public ScrollBox {
public:
    ZAF_OBJECT;

public:
    TreeControl();
    ~TreeControl();

    void SetDataSource(const std::weak_ptr<TreeDataSource>& data_source);
    void SetDelegate(const std::weak_ptr<TreeControlDelegate>& delegate);

    bool AutoAdjustScrollBarSmallChange() const;
    void SetAutoAdjustScrollBarSmallChange(bool value);

    void SetSelectionMode(ListSelectionMode selection_mode);

    std::vector<std::shared_ptr<Object>> GetAllSelectedItems() const;
    std::shared_ptr<Object> GetFirstSelectedItem() const;

    void SelectItem(const std::shared_ptr<Object>& data);
    void UnselectItem(const std::shared_ptr<Object>& data);

    void ExpandItem(const std::shared_ptr<Object>& data);
    void CollapseItem(const std::shared_ptr<Object>& data);

    void ScrollToItem(const std::shared_ptr<Object>& data);

    void ReloadItem(const std::shared_ptr<Object>& data);

    Observable<TreeControlSelectionChangeInfo> SelectionChangeEvent() const;
    Observable<TreeControlItemExpandInfo> ItemExpandEvent() const;
    Observable<TreeControlItemCollapseInfo> ItemCollapseEvent() const;

protected:
    void Initialize() override;
    void Layout(const zaf::Rect&) override;

    void OnMouseDown(const MouseDownInfo& event_info) override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;
    void OnMouseUp(const MouseUpInfo& event_info) override;
    void OnKeyDown(const KeyDownInfo& event_info) override;

    void OnFocusGained(const FocusGainedInfo& event_info) override;
    void OnFocusLost(const FocusLostInfo& event_info) override;

    virtual void OnDataSourceChanged(const std::shared_ptr<TreeDataSource>& previous_data_source) { 
    
    }

    virtual void OnDelegateChanged(const std::shared_ptr<TreeControlDelegate>& previous_delegate) { 
    
    }

private:
    void SelectionChange();
    void ItemExpand(const std::shared_ptr<Object>& data);
    void ItemCollapse(const std::shared_ptr<Object>& data);

private:
    std::shared_ptr<TreeItemContainer> item_container_;
    std::weak_ptr<TreeDataSource> data_source_;
    std::weak_ptr<TreeControlDelegate> delegate_;
    std::shared_ptr<internal::TreeCore> core_;

    Event<TreeControlSelectionChangeInfo> selection_changed_event_;
    Event<TreeControlItemExpandInfo> item_expand_event_;
    Event<TreeControlItemCollapseInfo> item_collapse_event_;
};

ZAF_OBJECT_BEGIN(TreeControl);
ZAF_OBJECT_END;


class TreeControlEventInfo {
public:
    TreeControlEventInfo(const std::shared_ptr<TreeControl>& tree_control) : 
        tree_control_(tree_control) {

    }

    const std::shared_ptr<TreeControl>& TreeControl() const {
        return tree_control_;
    }

private:
    std::shared_ptr<zaf::TreeControl> tree_control_;
};


class TreeControlSelectionChangeInfo : public TreeControlEventInfo {
public:
    using TreeControlEventInfo::TreeControlEventInfo;
};


class TreeControlItemExpandInfo : public TreeControlEventInfo {
public:
    TreeControlItemExpandInfo(
        const std::shared_ptr<zaf::TreeControl>& tree_control, 
        const std::shared_ptr<Object>& item_data)
        : 
        TreeControlEventInfo(tree_control),
        item_data_(item_data) {

    }

    const std::shared_ptr<Object>& ItemData() const {
        return item_data_;
    }

private:
    std::shared_ptr<Object> item_data_;
};


class TreeControlItemCollapseInfo : public TreeControlEventInfo {
public:
    TreeControlItemCollapseInfo(
        const std::shared_ptr<zaf::TreeControl>& tree_control,
        const std::shared_ptr<Object>& item_data)
        :
        TreeControlEventInfo(tree_control),
        item_data_(item_data) {

    }

    const std::shared_ptr<Object>& ItemData() const {
        return item_data_;
    }

private:
    std::shared_ptr<Object> item_data_;
};

}