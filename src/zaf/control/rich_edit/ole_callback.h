#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/clipboard/data_object.h>
#include <zaf/control/rich_edit.h>

namespace zaf::rich_edit {

enum class ClipboardOperation {
    Drop = RECO_DROP,
    Paste = RECO_PASTE,
};


class OLECallback : NonCopyableNonMovable {
public:
    OLECallback() = default;
    virtual ~OLECallback() = default;

    std::shared_ptr<RichEdit> Host() const {
        return host_.lock();
    }

    virtual bool QueryAcceptData(
        const clipboard::DataObject& data_object,
        clipboard::FormatType& expected_format_type,
        ClipboardOperation operation,
        bool really_drop) = 0;

private:
    friend class RichEdit;

    void SetHost(std::weak_ptr<RichEdit> host) {
        host_ = std::move(host);
    }

private:
    std::weak_ptr<RichEdit> host_;
};

}