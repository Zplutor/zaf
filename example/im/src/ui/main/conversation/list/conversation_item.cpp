#include "ui/main/conversation/list/conversation_item.h"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <zaf/base/as.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/graphic/stroke_properties.h>
#include <zaf/graphic/text/text_format_properties.h>
#include "logic/service.h"
#include "ui/main/conversation/common_definition.h"
#include "ui/main/conversation/common/title_generating.h"

static std::wstring GenerateTimeDescription(std::time_t time);

static const float AvatarLeftMargin = 12;
static const float AvatarTopMargin = 11;
static const float AvatarRightMargin = 11;
static const float AvatarWidth = ConversationAvatarSize;
static const float AvatarHeight = ConversationAvatarSize;
static const float UnreadCountBubbleWidth = 24;
static const float UnreadCountBubbleHeight = 16;
static const float UnreadCountBubbleExtension = 7;
static const float TitleTopMargin = 12;
static const float TitleBottomMargin = 1;
static const float TitleRightMargin = 11;
static const float TitleHeight = 20;
static const float DigestHeight = 16;
static const float DigestRightMargin = 22;
static const float TimeRightMargin = 6;
static const float TimeWidth = 50;
static const float TimeHeight = 16;

void ConversationItem::Initialize() {

    __super::Initialize();

    InitializeAvatarView();
    InitializeUnreadCountBubble();
    InitializeTitleLabel();
    InitializeDigestLabel(); 
    InitializeTimeLabel();

    SetBackgroundColorPicker(zaf::ColorPicker([this](const zaf::Control& control) {

        if (IsSelectedInContext()) {
            return zaf::Color::FromRGB(0x6495CC);
        }

        if (conversation_->is_sticked) {
            return zaf::Color::FromRGB(0xE8F3FB);
        }

        if (ContainsMouse()) {
            return zaf::Color::FromRGB(0xF3F8FA);
        }

        return zaf::Color::White();
    }));
}


void ConversationItem::InitializeAvatarView() {

    avatar_view_ = zaf::Create<AvatarView>();
    avatar_view_->SetRect(zaf::Rect(AvatarLeftMargin, AvatarTopMargin, AvatarWidth, AvatarWidth));
    AddChild(avatar_view_);
}


void ConversationItem::InitializeUnreadCountBubble() {

    unread_count_bubble_ = zaf::Create<UnreadCountBubble>();
    unread_count_bubble_->SetRect(zaf::Rect(
        AvatarLeftMargin + AvatarWidth - UnreadCountBubbleWidth + UnreadCountBubbleExtension,
        AvatarTopMargin - UnreadCountBubbleExtension,
        UnreadCountBubbleWidth,
        UnreadCountBubbleHeight));

    AddChild(unread_count_bubble_);
}


void ConversationItem::InitializeTitleLabel() {

    title_label_ = zaf::Create<zaf::Label>();
    title_label_->SetRect(zaf::Rect(
        AvatarLeftMargin + AvatarWidth + AvatarRightMargin, 
        TitleTopMargin,
        200,
        TitleHeight));

    Subscriptions() += title_label_->StyleUpdateEvent().Subscribe(
        [](const zaf::StyleUpdateInfo& event_info) {
    
        auto label = zaf::As<zaf::Label>(event_info.Source());
        auto text_color = label->IsSelectedInContext() ? zaf::Color::White() : zaf::Color::Black();
        label->SetTextColor(text_color);
    });

    zaf::Font font;
    font.family_name = L"Î¢ÈíÑÅºÚ";
    font.size = 14;
    title_label_->SetFont(font);
    title_label_->SetTextTrimming(zaf::TextTrimmingGranularity::Character);

    AddChild(title_label_);
}


void ConversationItem::InitializeDigestLabel() {

    digest_label_ = zaf::Create<zaf::Label>();
    digest_label_->SetRect(zaf::Rect(
        AvatarLeftMargin + AvatarWidth + AvatarRightMargin,
        TitleTopMargin + TitleHeight + TitleBottomMargin,
        200,
        DigestHeight));

    Subscriptions() += digest_label_->StyleUpdateEvent().Subscribe(
        [this](const zaf::StyleUpdateInfo& event_info) {
    
        auto label = zaf::As<zaf::Label>(event_info.Source());
        auto text_color = 
            label->IsSelectedInContext() ? 
            zaf::Color::White() : 
            zaf::Color::FromRGB(0x979797);
        label->SetTextColor(text_color);
    });

    zaf::Font font;
    font.family_name = L"Î¢ÈíÑÅºÚ";
    font.size = 12;
    digest_label_->SetFont(font);
    digest_label_->SetTextTrimming(zaf::TextTrimmingGranularity::Character);

    AddChild(digest_label_);
}


void ConversationItem::InitializeTimeLabel() {

    time_label_ = zaf::Create<zaf::Label>();
    time_label_->SetTextAlignment(zaf::TextAlignment::Tailing);

    Subscriptions() += time_label_->StyleUpdateEvent().Subscribe(
        [](const zaf::StyleUpdateInfo& event_info) {
    
        auto label = zaf::As<zaf::Label>(event_info.Source());
        auto text_color =
            label->IsSelectedInContext() ? 
            zaf::Color::White() :
            zaf::Color::FromRGB(0xb6b6b6);
        label->SetTextColor(text_color);
    });

    zaf::Font font;
    font.family_name = L"Î¢ÈíÑÅºÚ";
    font.size = 11;
    time_label_->SetFont(font);

    AddChild(time_label_);
}


void ConversationItem::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    const auto& current_size = ContentSize();

    auto time_label_require_size = time_label_->CalculatePreferredSize();

    time_label_->SetRect(zaf::Rect(
        current_size.width - TimeRightMargin - time_label_require_size.width,
        TitleTopMargin,
        time_label_require_size.width,
        TimeHeight));

    title_label_->SetWidth(
        time_label_->Position().x - 
        title_label_->Position().x - 
        TitleRightMargin);

    digest_label_->SetWidth(current_size.width - digest_label_->Position().x - DigestRightMargin);
}


void ConversationItem::Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) const {

    __super::Paint(canvas, dirty_rect);

    canvas.SetBrushWithColor(zaf::Color::FromRGB(0xE4E4E4));

    const auto& rect = Rect();
    float y = rect.size.height - 1;

    canvas.DrawLine(
        zaf::Point(-1, y), 
        zaf::Point(rect.size.width, y), 
        1);
}


void ConversationItem::LoadConversation(
    const std::shared_ptr<Conversation>& conversation,
    const zaf::wic::BitmapSource& avatar_image) {

    conversation_ = conversation;    

    avatar_view_->SetImageSource(avatar_image);

    auto unread_count = Service::GetInstance().GetConversationUnreadMessageCount(conversation_->id);
    unread_count_bubble_->SetUnreadCount(unread_count);
    unread_count_bubble_->SetIsMinimize(conversation_->is_silent);

    LoadTitle();
    LoadDigest();
    LoadTime();
}


void ConversationItem::LoadTitle() {
    title_label_->SetText(GenerateConversationTitle(conversation_));
}


void ConversationItem::LoadDigest() {

    auto last_message = Service::GetInstance().GetConversationLastMessage(conversation_->id);
    if (last_message == nullptr) {
        return;
    }

    std::wstring digest;
    if (conversation_->members.size() > 2) {
        auto sender = Service::GetInstance().GetUser(last_message->sender_id);
        digest.append(sender->name).append(L": ");
    }
    digest.append(last_message->content);

    digest_label_->SetText(digest);
}


void ConversationItem::LoadTime() {
    time_label_->SetText(GenerateTimeDescription(conversation_->last_updated_time));
}


static std::wstring GenerateTimeDescription(std::time_t time) {

    auto current_time = std::time(nullptr);
    auto difference = current_time - time;

    if (difference < 60) {
        return L"just now";
    }

    if (difference < 60 * 60) {
        return std::to_wstring(difference / 60) + L" minutes ago";
    }

    std::tm current_tm = *std::localtime(&current_time);
    std::time_t today_zero_hour_time = 
        current_time - 
        current_tm.tm_hour * 60 * 60 - current_tm.tm_min * 60 - current_tm.tm_sec;

    std::tm tm = *std::localtime(&time);

    if (time >= today_zero_hour_time) {

        std::wostringstream description_stream;
        description_stream 
            << std::setw(2) << std::setfill(L'0') << tm.tm_hour
            << L':'
            << std::setw(2) << std::setfill(L'0') << tm.tm_min;
        return description_stream.str();
    }

    std::time_t yesterday_zero_hour_time = today_zero_hour_time - 24 * 60 * 60;
    if (time >= yesterday_zero_hour_time) {
        return L"yesterday";
    }

    std::time_t a_week_ago_zero_hour_time = today_zero_hour_time - 7 * 24 * 60 * 60;
    if (time >= a_week_ago_zero_hour_time) {

        switch (tm.tm_wday) {
        case 0: return L"sunday";
        case 1: return L"monday";
        case 2: return L"tuesday";
        case 3: return L"wednesday";
        case 4: return L"thursday";
        case 5: return L"friday";
        case 6: return L"saturday";
        default: break; //fall through
        }
    }

    std::time_t current_year_zero_hour_time = 
        current_time - 
        current_tm.tm_yday * 24 * 60 * 60 - 
        current_tm.tm_hour * 60 * 60 - 
        current_tm.tm_min * 60 - 
        current_tm.tm_sec;

    if (time >= current_year_zero_hour_time) {

        std::wostringstream description_stream;
        description_stream
            << std::setw(2) << std::setfill(L'0') << tm.tm_mon + 1 << '/'
            << std::setw(2) << std::setfill(L'0') << tm.tm_mday;
        return description_stream.str();
    }

    std::wostringstream description_stream;
    description_stream
        << tm.tm_year + 1900 << '/'
        << std::setw(2) << std::setfill(L'0') << tm.tm_mon + 1 << '/'
        << std::setw(2) << std::setfill(L'0') << tm.tm_mday;

    auto description = description_stream.str();
    return description.substr(description.length() - 8);
}


void ConversationItem::UnreadCountBubble::Initialize() {

    __super::Initialize();

    SetBackgroundColor(zaf::Color::Transparent());
}


void ConversationItem::UnreadCountBubble::Paint(
    zaf::Canvas& canvas, 
    const zaf::Rect& dirty_rect) const {

    __super::Paint(canvas, dirty_rect);

    if (unread_count_ == 0) {
        return;
    }

    if (! is_minimize_) {
        PaintNormalBubble(canvas);
    }
    else {
        PaintMinimizeBubble(canvas);
    }
}


void ConversationItem::UnreadCountBubble::PaintNormalBubble(zaf::Canvas& canvas) const {

    std::wstring unread_count_text;
    if (unread_count_ <= 99) {
        unread_count_text = std::to_wstring(unread_count_);
    }
    else {
        unread_count_text = L"99+";
    }

    zaf::RoundedRect rounded_rect;
    rounded_rect.x_radius = UnreadCountBubbleHeight / 2;
    rounded_rect.y_radius = rounded_rect.x_radius;

    if (unread_count_text.length() == 1) {

        rounded_rect.rect = zaf::Rect(
            UnreadCountBubbleWidth - UnreadCountBubbleHeight,
            0,
            UnreadCountBubbleHeight,
            UnreadCountBubbleHeight);
    }
    else {

        rounded_rect.rect = zaf::Rect(
            0,
            0,
            UnreadCountBubbleWidth,
            UnreadCountBubbleHeight);
    }

    canvas.SetBrushWithColor(zaf::Color::FromRGB(0xFF6251));
    canvas.DrawRoundedRectangle(rounded_rect);

    zaf::TextFormatProperties text_format_properties;
    text_format_properties.font_family_name = L"Î¢ÈíÑÅºÚ";
    text_format_properties.font_size = 10;
    text_format_properties.font_weight = zaf::FontWeight::Regular;
    auto text_format = zaf::GraphicFactory::Instance().CreateTextFormat(text_format_properties);
    if (!text_format) {
        return;
    }

    text_format.SetTextAlignment(zaf::TextAlignment::Center);
    text_format.SetParagraphAlignment(zaf::ParagraphAlignment::Center);
    text_format.SetWordWrapping(zaf::WordWrapping::NoWrap);

    canvas.SetBrushWithColor(zaf::Color::White());
    canvas.DrawTextFormat(unread_count_text, text_format, rounded_rect.rect);
}


void ConversationItem::UnreadCountBubble::PaintMinimizeBubble(zaf::Canvas& canvas) const {

    zaf::Ellipse ellipse(
        zaf::Point(UnreadCountBubbleWidth - UnreadCountBubbleHeight + 2 + 5, 2 + 6),
        5.5, 
        5.5);

    canvas.SetBrushWithColor(zaf::Color::FromRGB(0xFF6251));
    canvas.DrawEllipse(ellipse);
}