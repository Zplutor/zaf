#include <zaf/control/rich_edit/ole_callback.h>

namespace zaf::rich_edit {

OperationResult OLECallback::CanInsertClipboardData(
    ClipboardOperation operation,
    const clipboard::DataObject& data_object,
    clipboard::FormatType format_type) {

    return OperationResult::Pending;
}


OperationResult OLECallback::InsertClipboardData(
    ClipboardOperation operation,
    const clipboard::DataObject& data_object,
    clipboard::FormatType& format_type) {

    return OperationResult::Pending;
}


OperationResult OLECallback::GetClipboardData(
    ClipboardOperation operation,
    const TextRange& text_range,
    clipboard::DataObject& data_object) {

    return OperationResult::Pending;
}

}