#pragma once

#include <zaf/control/image_box.h>
#include <zaf/control/label.h>
#include <zaf/control/list_control.h>
#include "conversation.h"

class ConversationItem : public zaf::ListControl::Item {
public:
    void LoadConversation(const std::shared_ptr<Conversation>& conversation);

    void Initialize() override;

protected:
    void Layout(const zaf::Rect& previous_rect) override;
    void Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) override;

    void MouseEnter() override {
        NeedRepaint();
    }

    void MouseLeave() override {
        NeedRepaint();
    }

private:
    class UnreadCountBubble : public zaf::Control {
    public:
        void Initialize() override;

        void SetUnreadCount(std::size_t unread_count) {
            unread_count_ = unread_count;
            NeedRepaint();
        }

        void SetIsMinimize(bool is_minimize) {
            is_minimize_ = is_minimize;
        }

    protected:
        void Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) override;

    private:
        void PaintNormalBubble(zaf::Canvas& canvas);
        void PaintMinimizeBubble(zaf::Canvas& canvas);

    private:
        std::size_t unread_count_ = 0;
        bool is_minimize_ = false;
    };

private:
    void InitializeAvatarBox();
    void InitializeUnreadCountBubble();
    void InitializeTitleLabel();
    void InitializeDigestLabel();
    void InitializeTimeLabel();

    void LoadTitle();
    void LoadDigest();
    void LoadTime();

private:
    std::shared_ptr<zaf::ImageBox> avatar_box_;
    std::shared_ptr<UnreadCountBubble> unread_count_bubble_;
    std::shared_ptr<zaf::Label> title_label_;
    std::shared_ptr<zaf::Label> digest_label_;
    std::shared_ptr<zaf::Label> time_label_;

    std::shared_ptr<Conversation> conversation_;
};