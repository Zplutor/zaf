#include "control/image_box/image_box_explore_manager.h"
#include <zaf/application.h>
#include <zaf/control/image_box.h>
#include <zaf/creation.h>
#include <zaf/graphic/resource_factory.h>
#include "control/control_operate_panels_creator.h"

void ImageBoxExploreManager::Initialize(
    std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_panels) {

    auto image_box = zaf::Create<zaf::ImageBox>();
    image_box->SetSize(zaf::Size(300, 300));

    auto image = zaf::GetResourceFactory()->CreateImage(L"C:\\Users\\Zplutor\\Desktop\\1987001170.jpg");
    if (image != nullptr) {
        image_box->SetImage(image);
    }
    control = image_box;

    CreateControlOperatePanels(image_box, operate_panels);
}