#include "manager/scroll_bar_explore_manager.h"

namespace {

void ChangeScrollBarSize(const std::shared_ptr<zaf::ScrollBar>& scroll_bar) {

    zaf::Size size;
    if (scroll_bar->IsHorizontal()) {
        size.width = 150;
        size.height = 20;
    }
    else {
        size.width = 20;
        size.height = 150;
    }

    scroll_bar->SetSize(size);
}

}

std::shared_ptr<zaf::Control> ScrollBarExploreManager::CreateExploredControl() {
    
    auto scroll_bar = zaf::Create<zaf::ScrollBar>();
    scroll_bar->SetIsHorizontal(true);
    scroll_bar->SetValueRange(0, 100);
    ChangeScrollBarSize(scroll_bar);
    return scroll_bar;
}
