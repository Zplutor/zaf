#include <zaf/internal/textual/text_box_clipboard_operation.h>
#include <zaf/base/as.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/clipboard/text_data.h>
#include <zaf/control/textual/styled_text_data.h>
#include <zaf/creation.h>

using namespace zaf::clipboard;
using namespace zaf::textual;

namespace zaf::internal {

void SaveStyledTextToClipboard(textual::StyledText styled_text) {

    StyledTextData clipboard_data{ std::move(styled_text) };

    auto data_object = DataObject::Create();

    //Unicode text format.
    data_object.SetData(DataDescriptor::FromFormatType(FormatType::Text), clipboard_data);

    //StyledText format.
    data_object.SetData(
        DataDescriptor::FromFormatType(StyledTextData::FormatType()),
        clipboard_data);

    Clipboard::SetDataObject(data_object);
}


textual::StyledText LoadStyledTextFromClipboard() {

    auto data_object = Clipboard::GetDataObject();

    try {
        StyledTextData styled_text_data;
        data_object.GetData(
            DataDescriptor::FromFormatType(StyledTextData::FormatType()),
            styled_text_data);

        return styled_text_data.Detach();
    }
    catch (...) {

    }

    TextData text_data;
    data_object.GetData(DataDescriptor::FromFormatType(FormatType::Text), text_data);
    return StyledText{ text_data.Detach() };
}

}