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


bool LoadStyledTextFromClipboard(textual::StyledText& styled_text, bool& is_styled_text) {

    auto data_object = Clipboard::GetDataObject();
    auto enumerator = data_object.EnumerateDataDescriptors();
    enumerator.Reset();

    do {

        auto descriptor = enumerator.Next();
        if (!descriptor) {
            break;
        }

        if (descriptor->FormatType() == StyledTextData::FormatType()) {
            try {
                StyledTextData styled_text_data;
                data_object.GetData(*descriptor, styled_text_data);
                styled_text = styled_text_data.Detach();
                is_styled_text = true;
                return true;
            }
            catch (...) {
                continue;
            }
        }
        else if (descriptor->FormatType() == FormatType::Text) {
            try {
                TextData text_data;
                data_object.GetData(*descriptor, text_data);
                styled_text.SetText(text_data.Detach());
                is_styled_text = false;
                return true;
            }
            catch (...) {
                continue;
            }
        }
    }
    while (true);

    return false;
}

}