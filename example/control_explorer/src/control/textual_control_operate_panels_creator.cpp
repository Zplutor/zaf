#include "control/textual_control_operate_panels_creator.h"
#include <algorithm>
#include <zaf/application.h>
#include <zaf/base/string/to_numeric.h>
#include <zaf/control/button.h>
#include <zaf/control/combo_box.h>
#include <zaf/control/label.h>
#include <zaf/control/layout/array_layouter.h>
#include <zaf/control/radio_button.h>
#include <zaf/control/text_box.h>
#include <zaf/creation.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/font/font_collection.h>
#include <zaf/graphic/font/font_family.h>
#include <zaf/graphic/localized_strings.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/property/clickable_control_property_tag.h>
#include <zaf/property/control_property_tag.h>
#include <zaf/property/setting.h>
#include <zaf/property/text_box_property_tag.h>
#include <zaf/property/textual_control_property_tag.h>
#include "control/common.h"

static std::shared_ptr<zaf::Control> CreateTextPanel(const std::shared_ptr<zaf::TextualControl>& textual_control);
static std::shared_ptr<zaf::Control> CreateFontPanel(const std::shared_ptr<zaf::TextualControl>& textual_control);
static std::shared_ptr<zaf::Control> CreateTextAlignmentPanel(const std::shared_ptr<zaf::TextualControl>& textual_control);
static std::shared_ptr<zaf::Control> CreateParagraphAlignmentPanel(const std::shared_ptr<zaf::TextualControl>& textual_control);
static std::shared_ptr<zaf::Control> CreateWordWrappingPanel(const std::shared_ptr<zaf::TextualControl>& textual_control);

void CreateTextualControlOperatePanels(
    const std::shared_ptr<zaf::TextualControl>& control,
    std::vector<std::shared_ptr<zaf::Control>>& operate_panels) {

    operate_panels.push_back(CreateTextPanel(control));
    operate_panels.push_back(CreateFontPanel(control));
    operate_panels.push_back(CreateTextAlignmentPanel(control));
    operate_panels.push_back(CreateParagraphAlignmentPanel(control));
    operate_panels.push_back(CreateWordWrappingPanel(control));
}


static std::shared_ptr<zaf::Control> CreateTextPanel(const std::shared_ptr<zaf::TextualControl>& textual_control) {

    auto panel = CreateOperateContainerPanel(3);

    auto text_label = zaf::Create<zaf::Label>();
    text_label->SetText(L"Text");
    text_label->SetMaximumWidth(OperatePanelLabelMaximumWidth);
    panel->AddChild(text_label);

    auto input_text_box = zaf::Create<zaf::TextBox>();
    input_text_box->SetText(textual_control->GetText());
    input_text_box->SetIsMultiline(true);
    input_text_box->SetAcceptReturn(true);
    panel->AddChild(input_text_box);

    auto set_text_button = CreateSetButton();
    set_text_button->GetClickEvent().AddListener(std::bind([textual_control, input_text_box]() {
        textual_control->SetText(input_text_box->GetText());
    }));
    panel->AddChild(set_text_button);

    textual_control->GetTextChangeEvent().AddListener([input_text_box](const std::shared_ptr<zaf::TextualControl>& textual_control) {
        input_text_box->SetText(textual_control->GetText());
    });

    return panel;
}


static std::shared_ptr<zaf::Control> CreateFontPanel(const std::shared_ptr<zaf::TextualControl>& textual_control) {

    struct Context {
        std::shared_ptr<zaf::ComboBox> font_family_combo_box;
        std::shared_ptr<zaf::TextBox> font_size_text_box;
        std::shared_ptr<zaf::TextBox> font_weight_text_box;
    };
    auto context = std::make_shared<Context>();

    return zaf::SetProperties(
        CreateOperateContainerPanel(3),

        zaf::child = zaf::CreateWithProperties<zaf::Label>(
            zaf::text = L"Font",
            zaf::maximumWidth = OperatePanelLabelMaximumWidth
        ),

        zaf::child = zaf::CreateWithProperties<zaf::Control>(
            zaf::layouter = zaf::GetVerticalArrayLayouter(),

            zaf::child = zaf::CreateWithProperties<zaf::Control>(
                zaf::layouter = zaf::GetHorizontalArrayLayouter(),
                zaf::child = zaf::CreateWithProperties<zaf::Label>(
                    zaf::text = L"Family",
                    zaf::maximumWidth = OperatePanelLabelMaximumWidth
                ),
                zaf::child = context->font_family_combo_box = []() {

                    auto font_name_combo_box = zaf::Create<zaf::ComboBox>();
                    font_name_combo_box->SetMaximumVisibleItemCount(10);

                    auto font_name_drop_down_list = font_name_combo_box->GetDropDownListBox();
                    font_name_drop_down_list->AddItemWithText(std::wstring());

                    wchar_t locale_name[1024] = { 0 };
                    LCIDToLocaleName(LANG_USER_DEFAULT, locale_name, 1024, 0);
                    std::wstring user_default_locale = locale_name;

                    auto font_collection = zaf::GetResourceFactory()->GetSystemFontCollection();

                    for (const auto& each_font_family : font_collection.GetEnumerator()) {

                        auto localized_strings = each_font_family.GetFamilyNames();
                        auto index = localized_strings.FindLocaleName(user_default_locale);
                        if (index == zaf::InvalidIndex) {
                            index = localized_strings.FindLocaleName(L"en-us");
                        }
                        if (index != zaf::InvalidIndex) {
                            font_name_drop_down_list->AddItemWithText(localized_strings.GetString(index));
                        }
                    }

                    return font_name_combo_box;
                }()
            ),

            zaf::child = zaf::CreateWithProperties<zaf::Control>(
                zaf::layouter = zaf::GetHorizontalArrayLayouter(),
                zaf::child = zaf::CreateWithProperties<zaf::Label>(
                    zaf::text = L"Size",
                    zaf::maximumWidth = OperatePanelLabelMaximumWidth
                ),
                zaf::child = context->font_size_text_box = zaf::CreateWithProperties<zaf::TextBox>(
                    zaf::text = std::to_wstring(textual_control->GetFont().size),
                    zaf::textValidator = zaf::GetNumberTextValidator()
                )
            ),

            zaf::child = zaf::CreateWithProperties<zaf::Control>(
                zaf::layouter = zaf::GetHorizontalArrayLayouter(),
                zaf::child = zaf::CreateWithProperties<zaf::Label>(
                    zaf::text = L"Weight", 
                    zaf::maximumWidth = OperatePanelLabelMaximumWidth
                ),
                zaf::child = context->font_weight_text_box = zaf::CreateWithProperties<zaf::TextBox>(
                    zaf::text = std::to_wstring(textual_control->GetFont().weight),
                    zaf::textValidator = zaf::GetNumberTextValidator()
                )
            )
        ),

        zaf::child = zaf::SetProperties(
            CreateSetButton(), 
            zaf::onClick = std::bind([context, textual_control]() {

                zaf::Font new_font = zaf::Font::GetDefault();
                
                auto family_name = context->font_family_combo_box->GetText();
                if (! family_name.empty()) {
                    new_font.family_name = family_name;
                }
                new_font.size = zaf::ToNumeric<float>(context->font_size_text_box->GetText());

                auto weight = zaf::ToNumeric<int>(context->font_weight_text_box->GetText());
                new_font.weight = std::max(zaf::FontWeight::Minimum, std::min(zaf::FontWeight::Maximum, weight));
                textual_control->SetFont(new_font);
            })
        )
    );
}


static std::shared_ptr<zaf::Control> CreateTextAlignmentPanel(const std::shared_ptr<zaf::TextualControl>& textual_control) {

    auto panel = zaf::Create<zaf::Control>();
    panel->SetSize(zaf::Size(0, SingleLineOperatePanelHeight));
    panel->SetLayouter(zaf::GetHorizontalArrayLayouter());

    auto label = zaf::Create<zaf::Label>();
    label->SetText(L"Text alignment");
    panel->AddChild(label);

    auto radio_button_group = std::make_shared<zaf::RadioButton::Group>();

    auto create_radio_button = [&](const std::wstring& text, zaf::TextAlignment text_alignment) {
    
        auto radio_button = zaf::Create<zaf::RadioButton>();
        radio_button->SetText(text);
        radio_button->SetGroup(radio_button_group);
        radio_button->GetSelectStateChangeEvent().AddListener([textual_control, text_alignment](const std::shared_ptr<zaf::RadioButton>& radio_button) {
            if (radio_button->IsSelected()) {
                textual_control->SetTextAlignment(text_alignment);
            }
        });

        if (textual_control->GetTextAlignment() == text_alignment) {
            radio_button->SetSelected();
        }

        panel->AddChild(radio_button);
    };

    create_radio_button(L"Leading", zaf::TextAlignment::Leading);
    create_radio_button(L"Tailing", zaf::TextAlignment::Tailing);
    create_radio_button(L"Center", zaf::TextAlignment::Center);
    create_radio_button(L"Justified", zaf::TextAlignment::Justified);

    return panel;
}


static std::shared_ptr<zaf::Control> CreateParagraphAlignmentPanel(const std::shared_ptr<zaf::TextualControl>& textual_control) {

    auto panel = zaf::Create<zaf::Control>();
    panel->SetSize(zaf::Size(0, SingleLineOperatePanelHeight));
    panel->SetLayouter(zaf::GetHorizontalArrayLayouter());

    auto label = zaf::Create<zaf::Label>();
    label->SetText(L"Paragraph alignment");
    panel->AddChild(label);

    auto radio_button_group = std::make_shared<zaf::RadioButton::Group>();

    auto create_radio_button = [&](const std::wstring& text, zaf::ParagraphAlignment paragraph_alignment) {

        auto radio_button = zaf::Create<zaf::RadioButton>();
        radio_button->SetText(text);
        radio_button->SetGroup(radio_button_group);
        radio_button->GetSelectStateChangeEvent().AddListener([textual_control, paragraph_alignment](const std::shared_ptr<zaf::RadioButton>& radio_button) {
            if (radio_button->IsSelected()) {
                textual_control->SetParagraphAlignment(paragraph_alignment);
            }
        });

        if (textual_control->GetParagraphAlignment() == paragraph_alignment) {
            radio_button->SetSelected();
        }

        panel->AddChild(radio_button);
    };

    create_radio_button(L"Near", zaf::ParagraphAlignment::Near);
    create_radio_button(L"Far", zaf::ParagraphAlignment::Far);
    create_radio_button(L"Center", zaf::ParagraphAlignment::Center);

    return panel;
}


static std::shared_ptr<zaf::Control> CreateWordWrappingPanel(const std::shared_ptr<zaf::TextualControl>& textual_control) {

    auto panel = zaf::Create<zaf::Control>();
    panel->SetSize(zaf::Size(0, SingleLineOperatePanelHeight));
    panel->SetLayouter(zaf::GetHorizontalArrayLayouter());

    auto label = zaf::Create<zaf::Label>();
    label->SetText(L"Word wrapping");
    panel->AddChild(label);

    auto radio_button_group = std::make_shared<zaf::RadioButton::Group>();

    auto create_radio_button = [&](const std::wstring& text, zaf::WordWrapping word_wrapping) {
    
        auto radio_button = zaf::Create<zaf::RadioButton>();
        radio_button->SetText(text);
        radio_button->SetGroup(radio_button_group);
        radio_button->GetSelectStateChangeEvent().AddListener([textual_control, word_wrapping](const std::shared_ptr<zaf::RadioButton>& radio_button) {
            if (radio_button->IsSelected()) {
                textual_control->SetWordWrapping(word_wrapping);
            }
        });

        if (textual_control->GetWordWrapping() == word_wrapping) {
            radio_button->SetSelected();
        }

        panel->AddChild(radio_button);
    };

    create_radio_button(L"No wrap", zaf::WordWrapping::NoWrap);
    create_radio_button(L"Wrap", zaf::WordWrapping::Wrap);
    create_radio_button(L"Whole word", zaf::WordWrapping::WholeWord);
    create_radio_button(L"Emergency break", zaf::WordWrapping::EmergencyBreak);
    create_radio_button(L"Character", zaf::WordWrapping::Character);

    return panel;
}