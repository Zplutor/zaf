#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/clipboard/data_object.h>
#include <zaf/control/rich_edit.h>

namespace zaf::rich_edit {

enum class ClipboardOperation {
    Copy = RECO_COPY,
    Cut = RECO_CUT,
    Paste = RECO_PASTE,
    Drop = RECO_DROP,
};


enum class OperationResult {
    OK,
    Cancel,
    Pending,
};


class OLECallback : NonCopyableNonMovable {
public:
    OLECallback() = default;
    virtual ~OLECallback() = default;

    std::shared_ptr<RichEdit> Host() const {
        return host_.lock();
    }

    /**
    Determines whether the rich edit can insert specified clipboard data.

    @param operation
        The operation which is processing. It can be ClipboardOperation::Paste or
        ClipboardOperation::Drop.

    @param data_object
        The data object which is being operated.

    @param format_type
        The format type of data object which is expected to used.

    @return 
        Returns OperationResult::OK to indicate that the rich edit can insert the data.
        Returns OperationResult::Cancel to indicate that the rich edit can't insert the data and 
        the operation should be cancelled.
        Returns OperationResult::Pending to indicate that the rich edit uses its default checking 
        to determine.
    */
    virtual OperationResult CanInsertClipboardData(
        ClipboardOperation operation,
        const clipboard::DataObject& data_object,
        clipboard::FormatType format_type);

    /**
    Inserts specified clipboard data to the rich edit.

    @param operation
        The operation which is processing. It can be ClipboardOperation::Paste or
        ClipboardOperation::Drop.

    @param data_object
        The data object which is being inserted.

    @param format_type
        The format type of data object which is expected to used. If OperationResult::Pending is 
        returned and format_type is modified, the default handling will insert the data to the rich
        edit with specified format type.

    @return
        Returns OperationResult::OK to indicate that the data has been inserted to the rich edit
        and to suppress the default handling.
        Returns OperationResult::Cancel to indicate that the data can't be inserted and to cancel 
        the operation.
        Returns OperationResult::Pending to indicate to use default handling to insert the data.
    */
    virtual OperationResult InsertClipboardData(
        ClipboardOperation operation,
        const clipboard::DataObject& data_object,
        clipboard::FormatType& format_type);

    /**
    Gets clipboard data for specified text range in rich edit.
    
    @param operation
        The operation which is processing. It can be ClipboardOperation::Copy or
        ClipboardOperation::Cut.

    @param text_range
        The text range.

    @param data_object
        The returned data object.

    @return
        Returns OperationResult::OK to indicate that the data object has been returned.
        Returns OperationResult::Cancel to indicate that the data object is failed to get and to 
        cancel the operation.
        Returns OperationResult::Pending to indicate to use default handling to get clipboard data.
    */
    virtual OperationResult GetClipboardData(
        ClipboardOperation operation,
        const Range& text_range,
        clipboard::DataObject& data_object);

private:
    friend class RichEdit;

    void SetHost(std::weak_ptr<RichEdit> host) {
        host_ = std::move(host);
    }

private:
    std::weak_ptr<RichEdit> host_;
};

}