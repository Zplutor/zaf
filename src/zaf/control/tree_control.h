#pragma once

#include <zaf/base/define.h>
#include <zaf/control/list_control.h>

namespace zaf {

class TreeItemSource;

class TreeControl : public ListControl {
public:

protected:
    

private:
    std::shared_ptr<TreeItemSource> item_source_;
};

class TreeItem : public ListItem {
public:
  
};

}