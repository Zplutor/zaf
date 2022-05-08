#include <gtest/gtest.h>
#include <fstream>
#include <boost/algorithm/hex.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <zaf/base/stream.h>
#include <zaf/resource/internal/resource_loader.h>

using namespace zaf;
using namespace zaf::internal;

namespace {

std::string CalculateMD5(void* buffer, std::size_t size) {

    boost::uuids::detail::md5 md5;
    md5.process_bytes(buffer, size);

    boost::uuids::detail::md5::digest_type digest;
    md5.get_digest(digest);

    std::string hex_string;
    boost::algorithm::hex(std::begin(digest), std::end(digest), std::back_inserter(hex_string));

    return hex_string;
}


std::string CalculateFileMD5(const std::wstring& file) {

    std::filesystem::path file_path(__FILE__);
    file_path = file_path.parent_path().parent_path().parent_path().parent_path();
    file_path = file_path / "resource" / file;

    std::ifstream file_stream(file_path, std::ios::binary);
    if (!file_stream) {
        return {};
    }

    file_stream.seekg(0, std::ios::end);
    auto file_size = file_stream.tellg();
    auto buffer = std::make_unique<char[]>(file_size);

    file_stream.seekg(0, std::ios::beg);
    file_stream.read(buffer.get(), file_size);
    
    return CalculateMD5(buffer.get(), file_size);
}


std::string CalculateStreamMD5(const Stream& stream) {

    auto buffer = std::make_unique<char[]>(stream.GetLength());
    stream.Read(stream.GetLength(), buffer.get());

    return CalculateMD5(buffer.get(), stream.GetLength());
}

}


TEST(ResourceLoaderTest, IsImageResource) {

    ASSERT_TRUE(ResourceLoader::IsImageResource(L"logo.png"));
    ASSERT_TRUE(ResourceLoader::IsImageResource(L"logo.bmp"));
    ASSERT_TRUE(ResourceLoader::IsImageResource(L"logo.gif"));
    ASSERT_TRUE(ResourceLoader::IsImageResource(L"logo.jpg"));
    ASSERT_TRUE(ResourceLoader::IsImageResource(L"logo.jpeg"));
    ASSERT_TRUE(ResourceLoader::IsImageResource(L"logo.pNg"));
    ASSERT_TRUE(ResourceLoader::IsImageResource(L"resources/logo.png"));

    ASSERT_FALSE(ResourceLoader::IsImageResource(L""));
    ASSERT_FALSE(ResourceLoader::IsImageResource(L"logo"));
    ASSERT_FALSE(ResourceLoader::IsImageResource(L"window.xml"));
}


TEST(ResourceLoaderTest, ConvertDPIToScalePercent) {

    ASSERT_EQ(ResourceLoader::ConvertDPIToScalePercent(0), 100);
    ASSERT_EQ(ResourceLoader::ConvertDPIToScalePercent(50), 100);
    ASSERT_EQ(ResourceLoader::ConvertDPIToScalePercent(96), 100);   //Standard
    ASSERT_EQ(ResourceLoader::ConvertDPIToScalePercent(100), 125);
    ASSERT_EQ(ResourceLoader::ConvertDPIToScalePercent(120), 125);  //Standard
    ASSERT_EQ(ResourceLoader::ConvertDPIToScalePercent(140), 150);
    ASSERT_EQ(ResourceLoader::ConvertDPIToScalePercent(144), 150);  //Standard
    ASSERT_EQ(ResourceLoader::ConvertDPIToScalePercent(150), 175);
    ASSERT_EQ(ResourceLoader::ConvertDPIToScalePercent(168), 175);  //Standard
    ASSERT_EQ(ResourceLoader::ConvertDPIToScalePercent(170), 200);
    ASSERT_EQ(ResourceLoader::ConvertDPIToScalePercent(192), 200);  //Standard
    ASSERT_EQ(ResourceLoader::ConvertDPIToScalePercent(200), 200);
    ASSERT_EQ(ResourceLoader::ConvertDPIToScalePercent(300), 200);
    ASSERT_EQ(ResourceLoader::ConvertDPIToScalePercent(500), 200);
}


TEST(ResourceLoaderTest, AddScalePercentToName) {

    ASSERT_EQ(ResourceLoader::AddScalePercentToName(L"logo.png", 100), L"logo.png");
    ASSERT_EQ(ResourceLoader::AddScalePercentToName(L"logo.png", 125), L"logo_%125.png");
    ASSERT_EQ(ResourceLoader::AddScalePercentToName(L"logo.png", 150), L"logo_%150.png");
    ASSERT_EQ(ResourceLoader::AddScalePercentToName(L"logo.png", 175), L"logo_%175.png");
    ASSERT_EQ(ResourceLoader::AddScalePercentToName(L"logo.png", 200), L"logo_%200.png");
    ASSERT_EQ(ResourceLoader::AddScalePercentToName(L"logo", 200), L"logo");
}


TEST(ResourceLoaderTest, CreateScalePercentSearchTable) {

    ASSERT_EQ(
        ResourceLoader::CreateScalePercentSearchTable(100),
        (std::vector<int>{ 125, 150, 175, 200 }));

    ASSERT_EQ(
        ResourceLoader::CreateScalePercentSearchTable(125),
        (std::vector<int>{ 150, 175, 200, 100 }));

    ASSERT_EQ(
        ResourceLoader::CreateScalePercentSearchTable(150),
        (std::vector<int>{ 175, 200, 125, 100 }));

    ASSERT_EQ(
        ResourceLoader::CreateScalePercentSearchTable(175),
        (std::vector<int>{ 200, 150, 125, 100 }));

    ASSERT_EQ(
        ResourceLoader::CreateScalePercentSearchTable(200),
        (std::vector<int>{ 175, 150, 125, 100 }));
}


TEST(ResourceLoaderTest, LoadNoneImage) {

    ResourceLoader loader;
    auto stream = loader.Load(L"", L"resource_loader_test/text.txt", 192.f);
    ASSERT_FALSE(stream.IsNull());
}


TEST(ResourceLoaderTest, LoadImageWithDPI) {

    ResourceLoader loader;

    auto test = [&loader](
        const std::wstring& file_name,
        const std::wstring& resource_name, 
        float dpi) {

        auto file_md5 = CalculateFileMD5(L"resource_loader_test/" + file_name);
        auto resource_stream = loader.Load(L"", L"resource_loader_test/" + resource_name, dpi);
        auto resource_md5 = CalculateStreamMD5(resource_stream);
        return file_md5 == resource_md5;
    };

    ASSERT_TRUE(test(L"test1.png", L"test1.png", 96));
    ASSERT_TRUE(test(L"test1_%125.png", L"test1.png", 120));
    ASSERT_TRUE(test(L"test1_%150.png", L"test1.png", 144));
    ASSERT_TRUE(test(L"test1_%175.png", L"test1.png", 168));
    ASSERT_TRUE(test(L"test1_%200.png", L"test1.png", 192));

    //Search forward.
    ASSERT_TRUE(test(L"test2_%175.png", L"test2.png", 144));

    //Search backward.
    ASSERT_TRUE(test(L"test3_%150.png", L"test3.png", 168));
}