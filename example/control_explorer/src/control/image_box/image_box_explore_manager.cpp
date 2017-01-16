#include "control/image_box/image_box_explore_manager.h"
#include <zaf/application.h>
#include <zaf/control/image_box.h>
#include <zaf/control/label.h>
#include <zaf/control/radio_button.h>
#include <zaf/creation.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/property/radio_button_property_tag.h>
#include <zaf/property/setting.h>
#include "control/common.h"
#include "control/control_operate_panels_creator.h"

static std::shared_ptr<zaf::Control> CreateInterpolationModeOperatePanel(const std::shared_ptr<zaf::ImageBox>& image_box);

void ImageBoxExploreManager::Initialize(
    std::shared_ptr<zaf::Control>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_panels) {

    auto image_box = zaf::Create<zaf::ImageBox>();
    image_box->SetSize(zaf::Size(300, 300));

    auto image = zaf::GetResourceFactory()->CreateImageDecoder(L"C:\\Users\\Zplutor\\Desktop\\1987001170.jpg");
    if (image != nullptr) {
        image_box->SetImageDecoder(image);
    }
    control = image_box;

    CreateControlOperatePanels(image_box, operate_panels);
    operate_panels.push_back(CreateInterpolationModeOperatePanel(image_box));
}


static std::shared_ptr<zaf::Control> CreateInterpolationModeOperatePanel(const std::shared_ptr<zaf::ImageBox>& image_box) {

    auto radio_button_group = std::make_shared<zaf::RadioButton::Group>();

    auto create_radio_button = [radio_button_group, image_box](const std::wstring& text, zaf::InterpolationMode interpolation_mode) {
        return zaf::CreateWithProperties<zaf::RadioButton>(
            zaf::text = text,
            zaf::group = radio_button_group,
            zaf::isSelected = image_box->GetInterpolationMode() == interpolation_mode,
            zaf::onSelectStateChange = std::bind([interpolation_mode, image_box]() {
                image_box->SetInterpolationMode(interpolation_mode);
            })
        );
    };

    return zaf::SetProperties(
        CreateOperateContainerPanel(1),
        zaf::child = zaf::CreateWithProperties<zaf::Label>(
            zaf::text = L"InterpolationMode",
            zaf::maximumWidth = OperatePanelLabelMaximumWidth),
        zaf::child = create_radio_button(L"NearestNeighbor", zaf::InterpolationMode::NearestNeighbor),
        zaf::child = create_radio_button(L"Linear", zaf::InterpolationMode::Linear)
    );
}