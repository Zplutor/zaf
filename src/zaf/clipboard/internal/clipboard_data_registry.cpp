#include <zaf/clipboard/internal/clipboard_data_registry.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/error/invalid_type_error.h>
#include <zaf/clipboard/clipboard_data.h>
#include <zaf/clipboard/drop_files_data.h>
#include <zaf/clipboard/text_data.h>
#include <zaf/control/textual/styled_text_clipboard_data.h>

namespace zaf::clipboard::internal {
namespace {

bool IsDerivedClipboardDataType(ObjectType* type) {

    if (type == ClipboardData::Type) {
        return false;
    }

    auto current = type;
    while (current) {
        if (current == ClipboardData::Type) {
            return true;
        }
        current = current->GetBase();
    }
    return false;
}

}

ClipboardDataRegistry& ClipboardDataRegistry::Instance() {
    static ClipboardDataRegistry instance;
    return instance;
}


ClipboardDataRegistry::ClipboardDataRegistry() {

    //Register standard format type data.
    registered_types_[FormatType::Text] = TextData::Type;
    registered_types_[FormatType::DropFiles] = DropFilesData::Type;

    //Format type that zaf uses.
    registered_types_[textual::StyledTextClipboardData::StyledTextFormatType()] =
        textual::StyledTextClipboardData::Type;
}


void ClipboardDataRegistry::RegisterType(FormatType format_type, ObjectType* data_type) {

    ZAF_EXPECT(data_type);

    if (!IsDerivedClipboardDataType(data_type)) {
        throw InvalidTypeError{ ZAF_SOURCE_LOCATION() };
    }

    registered_types_[format_type] = data_type;
}


ObjectType* ClipboardDataRegistry::GetRegisteredType(FormatType format_type) noexcept {

    auto object_type = Find(registered_types_, format_type);
    if (object_type) {
        return *object_type;
    }
    return nullptr;
}

}