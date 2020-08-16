#pragma once

#include <zaf/base/define.h>
#include <zaf/control/list_control.h>

namespace zaf {

class TreeItemSource;

class TreeControl : public ListControl {
public:

protected:
    void ItemSourceChange(const std::shared_ptr<ListItemSource>& previous_item_source) override;

private:
    std::shared_ptr<TreeItemSource> item_source_;
};

class TreeItem : public ListItem {
public:
  
};

class TreeItemSource : public ListItemSource {
public:
    virtual std::shared_ptr<TreeItem> CreateItem(const IndexPath& index_path) {
        return Create<TreeItem>();
    }

    std::size_t GetItemCount() override final;
    std::shared_ptr<ListItem> CreateItem(std::size_t index) override final;

private:
    
};

}