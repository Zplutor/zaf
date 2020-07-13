#include "property/color_property_item.h"
#include <zaf/base/string/case_conversion.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/base/string/to_string.h>
#include <zaf/control/combo_box.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/renderer/renderer.h>
#include <zaf/window/message/keyboard_message.h>

namespace {

class ColorComboBox : public zaf::ComboBox {
public:
    void Initialize() override {

        __super::Initialize();

        SetIsEditable(true);

        auto drop_down_list_box = GetDropDownListBox();
        drop_down_list_box->SetItemSource(std::make_shared<ColorItemSource>());
    }

    zaf::Color GetSelectedColor() const {
        return selected_color_;
    }

    void SetSelectedColor(const zaf::Color& color) {

        selected_color_ = color;

        bool has_selected = false;
        auto drop_down_list_box = GetDropDownListBox();
        for (std::size_t index = 0; index < drop_down_list_box->GetItemCount(); ++index) {

            auto color_item = std::dynamic_pointer_cast<ColorItem>(drop_down_list_box->GetItemAtIndex(index));
            if (color_item == nullptr) {
                continue;
            }

            if (color_item->GetColor() == selected_color_) {
                drop_down_list_box->SelectItemAtIndex(index);
                has_selected = true;
                break;
            }
        }
        
        if (has_selected) {
            return;
        }

        drop_down_list_box->UnselectAllItems();
        SetText(ColorToText(selected_color_));
    }

protected:
    void Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) override {

        __super::Paint(canvas, dirty_rect);
        PaintColorBox(canvas, GetContentRect(), selected_color_);
    }

    zaf::Rect GetTextRect() override {
        auto text_rect = __super::GetTextRect();
        text_rect.Deflate(zaf::Frame(ColorBoxWidth, 0, 0, 0));
        return text_rect;
    }

    void SelectionChange() override {

        __super::SelectionChange();

        auto drop_down_list_box = GetDropDownListBox();
        auto color_item = std::dynamic_pointer_cast<ColorItem>(drop_down_list_box->GetFirstSelectedItem());
        if (color_item != nullptr) {
            selected_color_ = color_item->GetColor();
            NeedRepaint();
            return;
        }

        auto color = TextToColor(GetText());
        if (color.has_value()) {
            selected_color_ = color.value();
        }
    }

    bool KeyDown(const zaf::KeyMessage& message) override {

        if (message.GetVirtualKey() == VK_RETURN) {

            if (GetDropDownListBox()->GetFirstSelectedItemIndex() == zaf::InvalidIndex) {
                SetText(ColorToText(selected_color_));
            }
            return true;
        }
        
        return __super::KeyDown(message);
    }

private:
    class ColorItemSource : public zaf::ListBoxItemSource {
    public:
        ColorItemSource() {
            for (const auto& each_info : GetColorInfos()) {
                AddItemWithText(each_info.name);
            }
        }

        std::shared_ptr<zaf::ListItem> CreateItem(std::size_t index) override {
            return zaf::Create<ColorItem>();
        }

        void LoadItem(std::size_t index, const std::shared_ptr<zaf::ListItem>& item) override {

            __super::LoadItem(index, item);

            auto color_item = std::dynamic_pointer_cast<ColorItem>(item);
            if (color_item != nullptr) {
                color_item->SetColor(GetColorInfos()[index].color);
            }
        }

    private:
        struct ColorInfoItem {
            std::wstring name;
            zaf::Color color;
        };

    private:
        static const std::vector<ColorInfoItem>& GetColorInfos() {
            static const std::vector<ColorInfoItem> color_infos{
                { L"White", zaf::Color::White },
                { L"Black", zaf::Color::Black },
                { L"Gray", zaf::Color::Gray },
                { L"Red", zaf::Color::Red },
                { L"Yellow", zaf::Color::Yellow },
                { L"Blue", zaf::Color::Blue },
                { L"Green", zaf::Color::Green },
                { L"Cyan", zaf::Color::Cyan },
                { L"Lime", zaf::Color::Lime },
                { L"Magenta", zaf::Color::Magenta },
                { L"Transparent", zaf::Color::Transparent },
            };
            return color_infos;
        }
    };

    class ColorItem : public zaf::ListItem {
    public:
        const zaf::Color& GetColor() const {
            return color_;
        }

        void SetColor(const zaf::Color& color) {
            color_ = color;
        }

    protected:
        void Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) override {

            __super::Paint(canvas, dirty_rect);
            PaintColorBox(canvas, GetContentRect(), color_);
        }

        zaf::Rect GetTextRect() override {
            auto text_rect = __super::GetTextRect();
            text_rect.Deflate(zaf::Frame(ColorBoxWidth, 0, 0, 0));
            return text_rect;
        }

    private:
        zaf::Color color_;
    };

private:
    static zaf::Rect GetColorBoxRect();

    static void PaintColorBox(zaf::Canvas& canvas, const zaf::Rect& content_rect, const zaf::Color& color) {

        zaf::Rect box_rect = content_rect;
        box_rect.Deflate(zaf::Frame(0, 0, content_rect.size.width - ColorBoxWidth, 0));
        box_rect.Deflate(4);

        canvas.SetBrushWithColor(color);
        canvas.DrawRectangle(box_rect);

        canvas.SetBrushWithColor(zaf::Color::Black);
        canvas.DrawRectangleFrame(box_rect, 1);
    }

    static std::wstring ColorToText(const zaf::Color& color) {

        auto float_to_hex = [](float value) {

            auto hex = zaf::ToWideString(
                static_cast<std::uint8_t>(value * 255),
                zaf::ToStringOptions().Base(16));

            if (hex.length() == 1) {
                hex = L'0' + hex;
            }
            return zaf::ToUppercased(hex);
        };

        std::wstring text(1, L'#');
        for (auto each_value : { color.a, color.r, color.g, color.b }) {
            text.append(float_to_hex(each_value));
        }

        return text;
    }

    static std::optional<zaf::Color> TextToColor(const std::wstring& text) {

        if (text.length() != 9) {
            return { };
        }

        if (text[0] != L'#') {
            return { };
        }

        zaf::ToNumericOptions to_numeric_options;
        to_numeric_options.Base(16);
        auto a = zaf::ToNumeric<std::uint8_t>(text.substr(1, 2), to_numeric_options);
        auto r = zaf::ToNumeric<std::uint8_t>(text.substr(3, 2), to_numeric_options);
        auto g = zaf::ToNumeric<std::uint8_t>(text.substr(5, 2), to_numeric_options);
        auto b = zaf::ToNumeric<std::uint8_t>(text.substr(7, 2), to_numeric_options);

        return zaf::Color( 
            float(r) / 255,
            float(g) / 255,
            float(b) / 255,
            float(a) / 255);
    }

private:
    static const float ColorBoxWidth;

private:
    zaf::Color selected_color_;
};


const float ColorComboBox::ColorBoxWidth = 30.f;

}


std::shared_ptr<PropertyItem> CreateColorPropertyItem(
    const std::wstring& title, 
    const std::function<zaf::Color()>& get_color,
    const std::function<void(const zaf::Color&)>& color_change) {

    auto color_combo_box = zaf::Create<ColorComboBox>();
    auto property_item = CreatePropertyItem(title, color_combo_box);

    color_combo_box->SetSelectedColor(get_color());
    color_combo_box->GetSelectionChangeEvent().AddListener([get_color, color_change](const std::shared_ptr<zaf::ComboBox>& combo_box) {

        auto color_combo_box = std::dynamic_pointer_cast<ColorComboBox>(combo_box);
        auto selected_color = color_combo_box->GetSelectedColor();
        color_change(selected_color);
        color_combo_box->SetSelectedColor(get_color());
    });

    return property_item;
}