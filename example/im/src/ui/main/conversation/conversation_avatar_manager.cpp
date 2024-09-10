#include "ui/main/conversation/conversation_avatar_manager.h"
#include <algorithm>
#include <zaf/application.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/wic/imaging_factory.h>
#include <zaf/graphic/graphic_factory.h>
#include "logic/service.h"
#include "ui/main/conversation/common_definition.h"
#include "ui/user_avatar_manager.h"

static std::vector<zaf::wic::BitmapSource> GetConversationMemberAvatarImages(const std::vector<Id>& member_ids);
static std::vector<zaf::Rect> GetAvatarCellRects(const zaf::Size& table_size, std::size_t cell_count);
static void GetLineCountAndRowCount(std::size_t cell_count, std::size_t& line_count, std::size_t& row_count);
static void DrawMemberAvatarImagesToConversationAvatarRenderer(
    const std::vector<zaf::wic::BitmapSource>& member_avatars,
    const std::vector<zaf::Rect>& member_avatar_rects,
    zaf::d2d::Renderer& renderer);

zaf::wic::BitmapSource ConversationAvatarManager::GetConversationAvatarImage(const std::shared_ptr<Conversation>& conversation) {

    auto iterator = conversation_avatar_images_.find(conversation->id);
    if (iterator == conversation_avatar_images_.end()) {

        auto avatar_image = GenerateConversationAvatarImage(conversation);
        iterator = conversation_avatar_images_.insert(std::make_pair(conversation->id, avatar_image)).first;
    }

    return iterator->second;
}


zaf::wic::BitmapSource ConversationAvatarManager::GenerateConversationAvatarImage(const std::shared_ptr<Conversation>& conversation) {

    Id current_user_id = Service::GetInstance().GetCurrentUserId();

    auto other_members = conversation->members;
    other_members.erase(
        std::remove(other_members.begin(), other_members.end(), current_user_id),
        other_members.end());

    if (other_members.empty()) {
        return zaf::wic::BitmapSource();
    }

    if (other_members.size() == 1) {
        return UserAvatarManager::GetInstance().GetUserAvatarImage(other_members.front());
    }

    return CombineMultiUserConversationAvatarImage(other_members);
}


zaf::wic::BitmapSource ConversationAvatarManager::CombineMultiUserConversationAvatarImage(const std::vector<Id>& member_ids) {

    zaf::Size image_size(ConversationAvatarSize, ConversationAvatarSize);

    zaf::wic::BitmapCreateOptions options;
    options.pixel_format = zaf::wic::PixelFormat::BGR32;
    auto image_source = zaf::wic::ImagingFactory::Instance().CreateBitmap(image_size, options);
    if (!image_source) {
        return zaf::wic::BitmapSource();
    }

    zaf::d2d::RendererProperties renderer_properties;
    auto renderer = 
        zaf::GraphicFactory::Instance().CreateBitmapRenderer(image_source, renderer_properties);
    if (!renderer) {
        return zaf::wic::BitmapSource();
    }

    auto member_avatar_images = GetConversationMemberAvatarImages(member_ids);
    auto member_avatar_cell_rects = GetAvatarCellRects(image_size, member_avatar_images.size());
    DrawMemberAvatarImagesToConversationAvatarRenderer(member_avatar_images, member_avatar_cell_rects, renderer);
   
    return image_source;
}


static std::vector<zaf::wic::BitmapSource> GetConversationMemberAvatarImages(const std::vector<Id>& member_ids) {

    auto& user_avatar_manager = UserAvatarManager::GetInstance();

    std::vector<zaf::wic::BitmapSource> member_avatar_images;
    for (auto each_id : member_ids) {

        auto image = user_avatar_manager.GetUserAvatarImage(each_id);
        if (!image) {
            continue;
        }

        member_avatar_images.push_back(image);
        if (member_avatar_images.size() >= 9) {
            break;
        }
    }

    return member_avatar_images;
}


static std::vector<zaf::Rect> GetAvatarCellRects(const zaf::Size& table_size, std::size_t cell_count) {

    const float gap_thickness = 2;
    float container_x = gap_thickness / 2;
    float container_y = gap_thickness / 2;
    float container_width = table_size.width - container_x * 2;
    float container_height = table_size.height - container_y * 2;

    std::size_t line_count = 0;
    std::size_t row_count = 0;
    GetLineCountAndRowCount(cell_count, line_count, row_count);
    
    float cell_length = 0;
    if (row_count > line_count) {
        cell_length = container_width / row_count;
    }
    else {
        cell_length = container_height / line_count;
    }

    std::vector<zaf::Rect> result;
    std::size_t remain_cell_count = cell_count;

    float top_gap = (container_height - line_count * cell_length) / 2;

    for (std::size_t current_line = line_count; current_line > 0; --current_line) {

        std::size_t cell_count_in_line = std::min(remain_cell_count, row_count);
        float left_gap = (container_width - cell_count_in_line * cell_length) / 2;

        for (std::size_t current_row = cell_count_in_line; current_row > 0; --current_row) {

            zaf::Rect cell_rect(
                container_x + left_gap + (current_row - 1) * cell_length,
                container_y + top_gap + (current_line - 1) * cell_length,
                cell_length,
                cell_length);

            cell_rect.Inflate(-1);

            result.push_back(cell_rect);
        }

        remain_cell_count -= cell_count_in_line;
    }

    std::reverse(result.begin(), result.end());
    return result;
}


static void GetLineCountAndRowCount(std::size_t cell_count, std::size_t& line_count, std::size_t& row_count) {

    line_count = 0;
    row_count = 2;
    while (true) {

        float temp_line_count = float(cell_count) / row_count;
        if (temp_line_count <= row_count + 1) {

            line_count = static_cast<std::size_t>(temp_line_count);
            if (line_count < temp_line_count) {
                ++line_count;
            }
            break;
        }

        ++row_count;
    }
}


static void DrawMemberAvatarImagesToConversationAvatarRenderer(
    const std::vector<zaf::wic::BitmapSource>& member_avatars,
    const std::vector<zaf::Rect>& member_avatar_rects,
    zaf::d2d::Renderer& renderer) {

    renderer.BeginDraw();
    renderer.Clear(zaf::Color::FromRGB(0xD8DADC));

    for (std::size_t index = 0; index < member_avatars.size(); ++index) {

        if (index >= member_avatar_rects.size()) {
            break;
        }

        auto avatar_rect = member_avatar_rects[index];
        avatar_rect = zaf::ToPixelAligned(avatar_rect, renderer.GetDPI());

        auto bitmap_scaler = zaf::wic::ImagingFactory::Instance().CreateBitmapScaler();
        bitmap_scaler.Initialize(
            member_avatars[index],
            avatar_rect.size,
            zaf::wic::ImageInterpolationMode::Fant);

        auto render_bitmap = renderer.CreateBitmap(bitmap_scaler);
        if (!render_bitmap) {
            continue;
        }

        renderer.DrawBitmap(
            render_bitmap, 
            avatar_rect,
            1.f, 
            zaf::d2d::InterpolationMode::Linear,
            nullptr);
    }

    renderer.EndDraw();
}