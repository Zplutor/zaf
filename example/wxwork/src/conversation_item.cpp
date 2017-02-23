#include "conversation_item.h"
#include <iomanip>
#include <sstream>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/image/image_decoder.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/graphic/stroke_properties.h>
#include <zaf/graphic/text/text_format_properties.h>

#include <zaf/base/log.h>

static std::wstring GenerateTimeDescription(std::time_t time);

static const float AvatarLeftMargin = 12;
static const float AvatarTopMargin = 11;
static const float AvatarRightMargin = 11;
static const float AvatarWidth = 40;
static const float AvatarHeight = 40;
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

    InitializeAvatarBox();
    InitializeUnreadCountBubble();
    InitializeTitleLabel();
    InitializeDigestLabel(); 
    InitializeTimeLabel();

    SetBackgroundColorPicker([this](const Control& control) {

        auto& item = dynamic_cast<const ConversationItem&>(control);

        if (item.IsSelected()) {
            return zaf::Color::FromRGB(0x6495CC);
        }
        
        if (conversation_->is_sticked) {
            return zaf::Color::FromRGB(0xE8F3FB);
        }

        if (item.GetContentRect().Contain(item.GetMousePosition())) {
            return zaf::Color::FromRGB(0xF3F8FA);
        }

        return zaf::Color::White;
    });
}


void ConversationItem::InitializeAvatarBox() {

    avatar_box_ = zaf::Create<zaf::ImageBox>();
    avatar_box_->SetRect(zaf::Rect(AvatarLeftMargin, AvatarTopMargin, AvatarWidth, AvatarWidth));
    AddChild(avatar_box_);
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

    title_label_->SetDefaultTextColorPicker([this](const zaf::Control& control) {
    
        if (IsSelected()) {
            return zaf::Color::White;
        }
        else {
            return zaf::Color::Black;
        }
    });

    zaf::Font font;
    font.family_name = L"微软雅黑";
    font.size = 14;
    title_label_->SetDefaultFont(font);

    AddChild(title_label_);
}


void ConversationItem::InitializeDigestLabel() {

    digest_label_ = zaf::Create<zaf::Label>();
    digest_label_->SetRect(zaf::Rect(
        AvatarLeftMargin + AvatarWidth + AvatarRightMargin,
        TitleTopMargin + TitleHeight + TitleBottomMargin,
        200,
        DigestHeight));

    digest_label_->SetDefaultTextColorPicker([this](const zaf::Control& control) {
    
        if (IsSelected()) {
            return zaf::Color::White;
        }
        else {
            return zaf::Color::FromRGB(0x979797);
        }
    });

    zaf::Font font;
    font.family_name = L"微软雅黑";
    font.size = 12;
    digest_label_->SetDefaultFont(font);

    AddChild(digest_label_);
}


void ConversationItem::InitializeTimeLabel() {

    time_label_ = zaf::Create<zaf::Label>();
    time_label_->SetTextAlignment(zaf::TextAlignment::Tailing);
    time_label_->SetDefaultTextColorPicker([this](const Control& control) {
    
        if (IsSelected()) {
            return zaf::Color::White;
        }
        else {
            return zaf::Color::FromRGB(0xb6b6b6);
        }
    });

    zaf::Font font;
    font.family_name = L"微软雅黑";
    font.size = 11;
    time_label_->SetDefaultFont(font);

    AddChild(time_label_);
}


void ConversationItem::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    const auto& current_size = GetContentSize();

    time_label_->SetRect(zaf::Rect(
        current_size.width - TimeRightMargin - TimeWidth,
        TitleTopMargin,
        TimeWidth,
        TimeHeight));

    title_label_->SetWidth(
        time_label_->GetPosition().x - 
        title_label_->GetPosition().x - 
        TitleRightMargin);

    digest_label_->SetWidth(current_size.width - digest_label_->GetPosition().x - DigestRightMargin);
}


void ConversationItem::Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    ZAF_LOG() << this << "Paint";

    canvas.SetBrushWithColor(zaf::Color::FromRGB(0xE4E4E4));

    const auto& rect = GetRect();
    float y = rect.size.height - 1;

    canvas.DrawLine(
        zaf::Point(-1, y), 
        zaf::Point(rect.size.width, y), 
        1);
}


void ConversationItem::LoadConversation(const std::shared_ptr<Conversation>& conversation) {

    conversation_ = conversation;
    
    auto avatar_decoder = zaf::GetResourceFactory()->CreateImageDecoder(conversation->members[0]->avatar_file_path);
    avatar_box_->SetImageDecoder(avatar_decoder);

    unread_count_bubble_->SetUnreadCount(conversation_->unread_count);
    unread_count_bubble_->SetIsMinimize(conversation_->is_silent);

    LoadTitle();
    LoadDigest();
    LoadTime();
}


void ConversationItem::LoadTitle() {

    if (! conversation_->title.empty()) {
        title_label_->SetText(conversation_->title);
        return;
    }
    
    if (conversation_->members.empty()) {
        title_label_->SetText(L"（无主题）");
        return;
    }

    std::wstring title;
    for (const auto& each_member : conversation_->members) {
        title.append(each_member->name);
        if (each_member != conversation_->members.back()) {
            title.append(1, L'、');
        }
    }

    title_label_->SetText(title);
}


void ConversationItem::LoadDigest() {

    const auto& digest = conversation_->digest;

    std::wstring digest_string;
    zaf::TextRange hightlight_range(0, 0);

    if (digest->has_confirm) {
        digest_string.append(L"[回执消息]");
        hightlight_range.length = digest_string.size();
    }

    if (digest->has_at) {
        digest_string.append(L"[有人@我]");
        hightlight_range.length = digest_string.size();
    }

    if (conversation_->members.size() <= 1) {
        digest_string.append(digest->content);
    }
    else {
        digest_string.append(digest->sender_name);
        digest_string.append(L": ");
        digest_string.append(conversation_->digest->content);
    }

    digest_label_->SetText(digest_string);
    digest_label_->SetTextColorAtRange(zaf::Color::FromRGB(0xD0021B), hightlight_range);
}


void ConversationItem::LoadTime() {
    time_label_->SetText(GenerateTimeDescription(conversation_->time));
}


static std::wstring GenerateTimeDescription(std::time_t time) {

    auto current_time = std::time(nullptr);
    auto difference = current_time - time;

    if (difference < 60) {
        return L"刚刚";
    }

    if (difference < 60 * 60) {
        return std::to_wstring(difference / 60) + L"分钟前";
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
        return L"昨天";
    }

    std::time_t a_week_ago_zero_hour_time = today_zero_hour_time - 7 * 24 * 60 * 60;
    if (time >= a_week_ago_zero_hour_time) {

        switch (tm.tm_wday) {
        case 0: return L"星期日";
        case 1: return L"星期一";
        case 2: return L"星期二";
        case 3: return L"星期三";
        case 4: return L"星期四";
        case 5: return L"星期五";
        case 6: return L"星期六";
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

    SetBackgroundColor(zaf::Color::Transparent);
}


void ConversationItem::UnreadCountBubble::Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) {

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


void ConversationItem::UnreadCountBubble::PaintNormalBubble(zaf::Canvas& canvas) {

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
    text_format_properties.font_family_name = L"微软雅黑";
    text_format_properties.font_size = 10;
    text_format_properties.font_weight = zaf::FontWeight::Regular;
    auto text_format = zaf::GetResourceFactory()->CreateTextFormat(text_format_properties);
    if (text_format == nullptr) {
        return;
    }

    text_format.SetTextAlignment(zaf::TextAlignment::Center);
    text_format.SetParagraphAlignment(zaf::ParagraphAlignment::Center);
    text_format.SetWordWrapping(zaf::WordWrapping::NoWrap);

    canvas.SetBrushWithColor(zaf::Color::White);
    canvas.DrawText(unread_count_text, text_format, rounded_rect.rect);
}


void ConversationItem::UnreadCountBubble::PaintMinimizeBubble(zaf::Canvas& canvas) {

    zaf::Ellipse ellipse(
        zaf::Point(UnreadCountBubbleWidth - UnreadCountBubbleHeight + 2 + 5, 2 + 6),
        5.5, 
        5.5);

    canvas.SetBrushWithColor(zaf::Color::FromRGB(0xFF6251));
    canvas.DrawEllipse(ellipse);
}