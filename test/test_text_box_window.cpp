#include "test_text_box_window.h"
#include <zaf/zaf.h>

#undef max

using namespace zaf;

class TestTextBoxWindow : public Window {
public:
	void Initialize() {

		SetTitle(L"test text box");
		SetRect(Rect(0, 0, 500, 500));

		CreateTextBoxContainer();
		CreateOptionsContainer();
	}

private:
	void CreateTextBoxContainer() {

		text_box_container_ = CreateControl<Control>();
		text_box_container_->SetLayouter(GetHorizontalArrayLayouter());
		text_box_container_->SetRect(Rect(0, 0, GetRect().size.width, 200));
		GetRootControl()->AddChild(text_box_container_);

		text_box_ = CreateControl<TextBox>();
		text_box_->SetText(L"TextBoxÎÄ±¾¿ò");
		text_box_container_->AddChild(text_box_);

		auto text_label = CreateControl<Label>();
		text_label->SetText(text_box_->GetText());
		text_box_->GetTextChangeEvent().AddListener([text_label](const std::shared_ptr<TextBox>& text_box) {
			text_label->SetText(text_box->GetText());
		});
		text_box_container_->AddChild(text_label);
	}


	void CreateOptionsContainer() {

		options_container_ = CreateControl<Control>();
		options_container_->SetLayouter(GetVerticalArrayLayouter());
		options_container_->SetRect(Rect(0, 200, GetRect().size.width, 200));
		GetRootControl()->AddChild(options_container_);

		CreateReadOnlyOption();
		CreateMaxLengthOption();
		CreatePasswordCharOption();
		CreateMultilineOption();
		CreateTextAlignmentOption();
		CreateWordWrapingOption();
		CreateSelectionRangeOption();
        CreateAllowBeepOption();
	}


	void CreateReadOnlyOption() {

		auto read_only_check_box = CreateControl<CheckBox>();
		read_only_check_box->SetText(L"Read only");
		read_only_check_box->SetIsChecked(text_box_->IsReadOnly());
		read_only_check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
			text_box_->SetIsReadOnly(check_box->IsChecked());
		});
		options_container_->AddChild(read_only_check_box);
	}


	void CreateMaxLengthOption() {

		auto max_length_check_box = CreateControl<CheckBox>();
		max_length_check_box->SetText(L"Max length 3");
		max_length_check_box->SetIsChecked(text_box_->GetMaximumLength() <= 3);
		max_length_check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
			text_box_->SetMaximumLength(check_box->IsChecked() ? 3 : std::numeric_limits<std::uint32_t>::max());
		});
		options_container_->AddChild(max_length_check_box);
	}

	
	void CreatePasswordCharOption() {

		auto container = CreateControl<Control>();
		container->SetLayouter(GetHorizontalArrayLayouter());
		options_container_->AddChild(container);

		auto use_password_char_check_box = CreateControl<CheckBox>();
		use_password_char_check_box->SetText(L"Use password character");
		use_password_char_check_box->SetIsChecked(text_box_->UsePasswordCharacter());
		use_password_char_check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
			text_box_->SetUsePasswordCharacter(check_box->IsChecked());
		});
		container->AddChild(use_password_char_check_box);

		auto password_char_text_box = CreateControl<TextBox>();
		password_char_text_box->SetMaximumLength(1);
		password_char_text_box->GetTextChangeEvent().AddListener([this](const std::shared_ptr<TextBox>& password_char_text_box) {
			auto text = password_char_text_box->GetText();
			if (! text.empty()) {
				text_box_->SetPasswordCharacter(text[0]);
			}
		});
		container->AddChild(password_char_text_box);
	}


	void CreateMultilineOption() {

		auto multiline_check_box = CreateControl<CheckBox>();
		multiline_check_box->SetText(L"Multiline");
		multiline_check_box->SetIsChecked(text_box_->IsMultiline());
		multiline_check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
			text_box_->SetIsMultiline(check_box->IsChecked());
		});
		options_container_->AddChild(multiline_check_box);
	}


	void CreateTextAlignmentOption() {

		auto container = CreateControl<Control>();
		container->SetLayouter(GetHorizontalArrayLayouter());
		options_container_->AddChild(container);

		auto label = CreateControl<Label>();
		label->SetText(L"TextAlignemnt: ");
		container->AddChild(label);

		auto select_change_event_callback = [this](const std::shared_ptr<RadioButton>& radio_button, TextAlignment text_alignment) {
			if (radio_button->IsSelected()) {
				text_box_->SetTextAlignment(text_alignment);
			}
		};

		auto group = std::make_shared<RadioButton::Group>();

		auto left_radio_button = CreateControl<RadioButton>();
		left_radio_button->SetText(L"Left");
		left_radio_button->SetGroup(group);
		left_radio_button->GetSelectStateChangeEvent().AddListener(
			std::bind(select_change_event_callback, std::placeholders::_1, TextAlignment::Leading)
		);
		container->AddChild(left_radio_button);

		auto center_radio_button = CreateControl<RadioButton>();
		center_radio_button->SetText(L"Center");
		center_radio_button->SetGroup(group);
		center_radio_button->GetSelectStateChangeEvent().AddListener(
			std::bind(select_change_event_callback, std::placeholders::_1, TextAlignment::Center)
		);
		container->AddChild(center_radio_button);

		auto right_radio_button = CreateControl<RadioButton>();
		right_radio_button->SetText(L"Right");
		right_radio_button->SetGroup(group);
		right_radio_button->GetSelectStateChangeEvent().AddListener(
			std::bind(select_change_event_callback, std::placeholders::_1, TextAlignment::Tailing)
		);
		container->AddChild(right_radio_button);
	}


	void CreateWordWrapingOption() {

		auto word_wrap_check_box = CreateControl<CheckBox>();
		word_wrap_check_box->SetText(L"Word wrap");
		word_wrap_check_box->SetIsChecked(text_box_->GetWordWrapping() != WordWrapping::NoWrap);
		word_wrap_check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
			text_box_->SetWordWrapping(check_box->IsChecked() ? WordWrapping::Wrap : WordWrapping::NoWrap);
		});
		options_container_->AddChild(word_wrap_check_box);
	}


	void CreateSelectionRangeOption() {

		auto container = CreateControl<Control>();
		container->SetLayouter(GetHorizontalArrayLayouter());
		options_container_->AddChild(container);

		auto label = CreateControl<Label>();
		label->SetText(L"Selection range:");
		container->AddChild(label);

		auto selection_position_text_box = CreateControl<TextBox>();
		text_box_->GetSelectionChangeEvent().AddListener([selection_position_text_box](const std::shared_ptr<TextBox>& text_box) {
			Range selection_range = text_box->GetSelectionRange();
			selection_position_text_box->SetText(std::to_wstring(selection_range.position));
		});
		container->AddChild(selection_position_text_box);

		auto selection_length_text_box = CreateControl<TextBox>();
		text_box_->GetSelectionChangeEvent().AddListener([selection_length_text_box](const std::shared_ptr<TextBox>& text_box) {
			Range selection_range = text_box->GetSelectionRange();
			selection_length_text_box->SetText(std::to_wstring(selection_range.length));
		});
		container->AddChild(selection_length_text_box);

		auto button = CreateControl<Button>();
		button->SetText(L"OK");
		button->GetClickEvent().AddListener([this, selection_position_text_box, selection_length_text_box](const std::shared_ptr<ClickableControl>& button) {
			
			Range new_range;

			auto position_string = selection_position_text_box->GetText();
			new_range.position = wcstoul(position_string.c_str(), nullptr, 10);
			
			auto length_string = selection_length_text_box->GetText();
			new_range.length = wcstoul(length_string.c_str(), nullptr, 10);

			text_box_->SetSelectionRange(new_range);
		});
		container->AddChild(button);
	}


    void CreateAllowBeepOption() {

        auto allow_beep_check_box = CreateControl<CheckBox>();
        allow_beep_check_box->SetText(L"Allow beep");
        allow_beep_check_box->SetIsChecked(text_box_->AllowBeep());
        allow_beep_check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
            text_box_->SetAllowBeep(check_box->IsChecked());
        });
        options_container_->AddChild(allow_beep_check_box);
    }

private:
	std::shared_ptr<Control> text_box_container_;
	std::shared_ptr<Control> options_container_;
	std::shared_ptr<TextBox> text_box_;
};


void ShowTestTextBoxWindow() {

	auto window = std::make_shared<TestTextBoxWindow>();
	window->Initialize();
	window->Show();
}
