#include "manager/spin_box_explore_manager.h"
#include <zaf/control/spin_box.h>
#include <zaf/creation.h>

std::shared_ptr<zaf::Control> SpinBoxExploreManager::CreateExploredControl() {

    auto result = zaf::Create<zaf::SpinBox>();
    result->SetSize(zaf::Size{ 60, 30 });
    result->SetMinValue(1);
    result->SetMaxValue(100);
    result->SetValue(50);
    return result;
}