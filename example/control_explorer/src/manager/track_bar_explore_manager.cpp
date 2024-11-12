#include "manager/track_bar_explore_manager.h"
#include <zaf/control/track_bar.h>

std::shared_ptr<zaf::Control> TrackBarExploreManager::CreateExploredControl() {

    auto track_bar = zaf::Create<zaf::TrackBar>();
    track_bar->SetIsHorizontal(true);
    track_bar->SetMinValue(0);
    track_bar->SetMaxValue(10);
    track_bar->SetFixedSize(zaf::Size{ 100, 30 });
    return track_bar;
}