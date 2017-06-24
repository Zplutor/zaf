#include <zaf/serialization/internal/built_in_types.h>
#include <zaf/control/button.h>
#include <zaf/control/check_box.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/combo_box.h>
#include <zaf/control/control.h>
#include <zaf/control/image_box.h>
#include <zaf/control/label.h>
#include <zaf/control/list_box.h>
#include <zaf/control/radio_button.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/split_control.h>
#include <zaf/control/text_box.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/ellipse.h>
#include <zaf/graphic/frame.h>
#include <zaf/graphic/point.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/rounded_rect.h>
#include <zaf/graphic/size.h>
#include <zaf/serialization/serializable_type.h>

namespace zaf {
namespace internal {

std::vector<std::shared_ptr<SerializableType>> GetBuiltInSerializableTypes() {

    return {
        CreateSerializableType<Button>(),
        CreateSerializableType<CheckBox>(),
        CreateSerializableType<ClickableControl>(),
        CreateSerializableType<Color>(),
        CreateSerializableType<ComboBox>(),
        CreateSerializableType<ConstantColorPicker>(),
        CreateSerializableType<Control>(),
        CreateSerializableType<Ellipse>(),
        CreateSerializableType<Frame>(),
        CreateSerializableType<ImageBox>(),
        CreateSerializableType<Label>(),
        CreateSerializableType<ListBox>(),
        CreateSerializableType<ListControl>(),
        CreateSerializableType<Point>(),
        CreateSerializableType<RadioButton>(),
        CreateSerializableType<Rect>(),
        CreateSerializableType<RoundedRect>(),
        CreateSerializableType<ScrollableControl>(),
        CreateSerializableType<ScrollBar>(),
        CreateSerializableType<Size>(),
        CreateSerializableType<SplitControl>(),
        CreateSerializableType<TextBox>(),
        CreateSerializableType<TextualControl>(),
    };
}

}
}