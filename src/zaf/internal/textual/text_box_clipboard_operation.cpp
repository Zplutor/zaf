#include <zaf/internal/textual/text_box_clipboard_operation.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/control/textual/styled_text_clipboard_data.h>
#include <zaf/creation.h>

using namespace zaf::clipboard;
using namespace zaf::textual;

namespace zaf::internal {

void SaveStyledTextToClipboard(textual::StyledText styled_text) {

    auto clipboard_data = Create<StyledTextClipboardData>(std::move(styled_text));

    auto data_object = DataObject::Create();

    //Unicode text format.
    data_object.SetData(clipboard_data, DataDescriptor::FromFormatType(FormatType::Text));

    //StyledText format.
    data_object.SetData(
        clipboard_data,
        DataDescriptor::FromFormatType(StyledTextClipboardData::StyledTextFormatType()));

    Clipboard::SetDataObject(data_object);
}

}