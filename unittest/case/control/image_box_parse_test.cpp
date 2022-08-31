#include <gtest/gtest.h>
#include <zaf/control/image_box.h>
#include <zaf/object/parsing/helpers.h>

namespace {

std::shared_ptr<zaf::ImageBox> CreateImageBoxFromXaml(const std::string& xaml) {
    return zaf::CreateObjectFromXaml<zaf::ImageBox>(xaml);
}

}

TEST(ImageBoxParseTest, ImageLayout) {

    auto image_box = CreateImageBoxFromXaml(R"(<ImageBox ImageLayout="Center" />)");
    ASSERT_NE(image_box, nullptr);
    ASSERT_EQ(image_box->ImageLayout(), zaf::ImageLayout::Center);

    image_box = CreateImageBoxFromXaml(R"(
        <ImageBox>
            <ImageBox.ImageLayout>Stretch</ImageBox.ImageLayout>
        </ImageBox>
    )");
    ASSERT_NE(image_box, nullptr);
    ASSERT_EQ(image_box->ImageLayout(), zaf::ImageLayout::Stretch);
}


TEST(ImageBoxParseTest, InterpolationMode) {

    auto image_box = CreateImageBoxFromXaml(R"(<ImageBox InterpolationMode="HighQualityCubic" />)");
    ASSERT_NE(image_box, nullptr);
    ASSERT_EQ(image_box->InterpolationMode(), zaf::InterpolationMode::HighQualityCubic);

    image_box = CreateImageBoxFromXaml(R"(
        <ImageBox>
            <ImageBox.InterpolationMode>Anisotropic</ImageBox.InterpolationMode>
        </ImageBox>
    )");
    ASSERT_NE(image_box, nullptr);
    ASSERT_EQ(image_box->InterpolationMode(), zaf::InterpolationMode::Anisotropic);
}