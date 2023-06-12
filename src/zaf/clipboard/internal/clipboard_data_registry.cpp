#include <zaf/clipboard/internal/clipboard_data_registry.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/error/check.h>
#include <zaf/clipboard/clipboard_data.h>
#include <zaf/clipboard/drop_files_data.h>
#include <zaf/clipboard/text_data.h>

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
}


void ClipboardDataRegistry::RegisterType(FormatType format_type, ObjectType* data_type) {

    ZAF_EXPECT(IsDerivedClipboardDataType(data_type));

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