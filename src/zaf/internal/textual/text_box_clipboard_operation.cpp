#include <zaf/internal/textual/text_box_clipboard_operation.h>
#include <zaf/base/as.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/control/textual/styled_text_clipboard_data.h>
#include <zaf/creation.h>

using namespace zaf::clipboard;
using namespace zaf::textual;

namespace zaf::internal {

void SaveStyledTextToClipboard(textual::StyledText styled_text) {

    StyledTextClipboardData clipboard_data{ std::move(styled_text) };

    auto data_object = DataObject::Create();

    //Unicode text format.
    data_object.SetData(DataDescriptor::FromFormatType(FormatType::Text), clipboard_data);

    //StyledText format.
    data_object.SetData(
        DataDescriptor::FromFormatType(StyledTextClipboardData::StyledTextFormatType()),
        clipboard_data);

    Clipboard::SetDataObject(data_object);
}


textual::StyledText LoadStyledTextFromClipboard() {

    auto data_object = Clipboard::GetDataObject();

    StyledTextClipboardData styled_text_data;
    data_object.GetData(
        DataDescriptor::FromFormatType(StyledTextClipboardData::StyledTextFormatType()),
        styled_text_data);

    return styled_text_data.Detach();
}

}