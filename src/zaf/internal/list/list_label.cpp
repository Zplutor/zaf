#include <zaf/internal/list/list_label.h>
#include <zaf/creation.h>
#include <zaf/internal/theme.h>

namespace zaf::internal {

std::shared_ptr<Label> CreateListLabel() {

    auto label = Create<zaf::Label>();
    label->SetPadding(Frame(2, 0, 2, 0));
    label->SetParagraphAlignment(dwrite::ParagraphAlignment::Center);
    label->SetTextColorPicker(ColorPicker([](const Control& control) {
        return control.IsSelectedInContext() ?
            Color::White() :
            Color::FromRGB(internal::ControlNormalTextColorRGB);
    }));
    return label;
}

}