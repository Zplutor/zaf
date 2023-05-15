#pragma once

#include <functional>
#include <string_view>
#include <variant>
#include <zaf/base/non_copyable.h>

namespace zaf {

class TextSourceChangeResult {
public:
    bool IsChanged() const {
        return is_changed_;
    }

    void SetIsChanged(bool value) {
        is_changed_ = value;
    }

    bool IsNotificationSent() const {
        return is_notification_sent_;
    }

    void SetIsNotificationSent(bool value) {
        is_notification_sent_ = value;
    }
    
private:
    bool is_changed_{};
    bool is_notification_sent_{};
};


class TextSource : NonCopyableNonMovable {
public:
    static std::unique_ptr<TextSource> Default();

public:
    TextSource() = default;
    virtual ~TextSource() = default;

    virtual std::size_t GetTextLength() = 0;
    virtual std::variant<std::wstring_view, std::wstring> GetText() = 0;
    virtual TextSourceChangeResult SetText(const std::wstring& text) = 0;
};

}