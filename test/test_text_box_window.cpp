#include "test_text_box_window.h"
#include <zaf/zaf.h>

#undef max

using namespace zaf;

class TestTextBoxWindow : public Window {
public:
	void Initialize() {

        __super::Initialize();

		SetTitle(L"test text box");
		SetRect(Rect(0, 0, 500, 700));

		CreateTextBoxContainer();
		CreateOptionsContainer();
	}

private:
	void CreateTextBoxContainer() {

		text_box_container_ = Create<Control>();
		text_box_container_->SetLayouter(GetHorizontalArrayLayouter());
		text_box_container_->SetRect(Rect(0, 0, GetRect().size.width, 200));
		GetRootControl()->AddChild(text_box_container_);

		text_box_ = Create<TextBox>();
		text_box_->SetText(L"TextBoxÎÄ±¾¿ò");

        text_box_scroll_container_ = Create<ScrollableControl>();
        text_box_scroll_container_->SetScrollContentControl(text_box_);
        text_box_container_->AddChild(text_box_scroll_container_);

		auto text_label = Create<Label>();
		text_label->SetText(text_box_->GetText());
		text_box_->GetTextChangeEvent().AddListener([text_label](const std::shared_ptr<TextBox>& text_box) {
			text_label->SetText(text_box->GetText());
		});
		text_box_container_->AddChild(text_label);
	}


	void CreateOptionsContainer() {

		options_container_ = Create<Control>();
		options_container_->SetLayouter(GetVerticalArrayLayouter());
		options_container_->SetRect(Rect(0, 200, GetRect().size.width, 300));
		GetRootControl()->AddChild(options_container_);

		CreateReadOnlyOption();
		CreateMaxLengthOption();
		CreatePasswordCharOption();
		CreateMultilineOption();
		CreateTextAlignmentOption();
        CreateParagraphAlignmentOption();
		CreateWordWrapingOption();
		CreateSelectionRangeOption();
        CreateAllowBeepOption();
        CreateAllowScrollOption();
        CreateAutoHideScrollbarOption();
        CreateHorizontalScrollOptions();
        CreateVerticalScrollOptions();
        CreateUndoOptions();
        CreateNumberOnlyOptions();
	}


	void CreateReadOnlyOption() {

		auto read_only_check_box = Create<CheckBox>();
		read_only_check_box->SetText(L"Read only");
		read_only_check_box->SetIsChecked(text_box_->IsReadOnly());
		read_only_check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
			text_box_->SetIsReadOnly(check_box->IsChecked());
		});
		options_container_->AddChild(read_only_check_box);
	}


	void CreateMaxLengthOption() {

		auto max_length_check_box = Create<CheckBox>();
		max_length_check_box->SetText(L"Max length 3");
		max_length_check_box->SetIsChecked(text_box_->GetMaximumLength() <= 3);
		max_length_check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
			text_box_->SetMaximumLength(check_box->IsChecked() ? 3 : std::numeric_limits<std::uint32_t>::max());
		});
		options_container_->AddChild(max_length_check_box);
	}

	
	void CreatePasswordCharOption() {

		auto container = Create<Control>();
		container->SetLayouter(GetHorizontalArrayLayouter());
		options_container_->AddChild(container);

		auto use_password_char_check_box = Create<CheckBox>();
		use_password_char_check_box->SetText(L"Use password character");
		use_password_char_check_box->SetIsChecked(text_box_->UsePasswordCharacter());
		use_password_char_check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
			text_box_->SetUsePasswordCharacter(check_box->IsChecked());
		});
		container->AddChild(use_password_char_check_box);

		auto password_char_text_box = Create<TextBox>();
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

		auto multiline_check_box = Create<CheckBox>();
		multiline_check_box->SetText(L"Multiline");
		multiline_check_box->SetIsChecked(text_box_->IsMultiline());
		multiline_check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
			text_box_->SetIsMultiline(check_box->IsChecked());
		});
		options_container_->AddChild(multiline_check_box);
	}


	void CreateTextAlignmentOption() {

		auto container = Create<Control>();
		container->SetLayouter(GetHorizontalArrayLayouter());
		options_container_->AddChild(container);

		auto label = Create<Label>();
		label->SetText(L"TextAlignemnt: ");
		container->AddChild(label);

		auto select_change_event_callback = [this](const std::shared_ptr<RadioButton>& radio_button, TextAlignment text_alignment) {
			if (radio_button->IsSelected()) {
				text_box_->SetTextAlignment(text_alignment);
			}
		};

		auto group = std::make_shared<RadioButton::Group>();

		auto left_radio_button = Create<RadioButton>();
		left_radio_button->SetText(L"Left");
		left_radio_button->SetGroup(group);
		left_radio_button->GetSelectStateChangeEvent().AddListener(
			std::bind(select_change_event_callback, std::placeholders::_1, TextAlignment::Leading)
		);
		container->AddChild(left_radio_button);

		auto center_radio_button = Create<RadioButton>();
		center_radio_button->SetText(L"Center");
		center_radio_button->SetGroup(group);
		center_radio_button->GetSelectStateChangeEvent().AddListener(
			std::bind(select_change_event_callback, std::placeholders::_1, TextAlignment::Center)
		);
		container->AddChild(center_radio_button);

		auto right_radio_button = Create<RadioButton>();
		right_radio_button->SetText(L"Right");
		right_radio_button->SetGroup(group);
		right_radio_button->GetSelectStateChangeEvent().AddListener(
			std::bind(select_change_event_callback, std::placeholders::_1, TextAlignment::Tailing)
		);
		container->AddChild(right_radio_button);

        switch (text_box_->GetTextAlignment()) {
            case TextAlignment::Leading:
                left_radio_button->SetSelected();
                break;
            case TextAlignment::Center:
                center_radio_button->SetSelected();
                break;
            case TextAlignment::Tailing:
                right_radio_button->SetSelected();
                break;
        }
	}


    void CreateParagraphAlignmentOption() {

        auto container = Create<Control>();
        container->SetLayouter(GetHorizontalArrayLayouter());
        options_container_->AddChild(container);

        auto label = Create<Label>();
        label->SetText(L"ParagraphAlignemnt: ");
        container->AddChild(label);

        auto select_change_event_callback = [this](const std::shared_ptr<RadioButton>& radio_button, ParagraphAlignment paragraph_alignment) {
            if (radio_button->IsSelected()) {
                text_box_->SetParagraphAlignment(paragraph_alignment);
            }
        };

        auto group = std::make_shared<RadioButton::Group>();

        auto top_radio_button = Create<RadioButton>();
        top_radio_button->SetText(L"Top");
        top_radio_button->SetGroup(group);
        top_radio_button->GetSelectStateChangeEvent().AddListener(
            std::bind(select_change_event_callback, std::placeholders::_1, ParagraphAlignment::Near));
        container->AddChild(top_radio_button);

        auto center_radio_button = Create<RadioButton>();
        center_radio_button->SetText(L"Center");
        center_radio_button->SetGroup(group);
        center_radio_button->GetSelectStateChangeEvent().AddListener(
            std::bind(select_change_event_callback, std::placeholders::_1, ParagraphAlignment::Center));
        container->AddChild(center_radio_button);

        auto bottom_radio_button = Create<RadioButton>();
        bottom_radio_button->SetText(L"Bottom");
        bottom_radio_button->SetGroup(group);
        bottom_radio_button->GetSelectStateChangeEvent().AddListener(
            std::bind(select_change_event_callback, std::placeholders::_1, ParagraphAlignment::Far));
        container->AddChild(bottom_radio_button);

        switch (text_box_->GetParagraphAlignment()) {
            case ParagraphAlignment::Near:
                top_radio_button->SetSelected();
                break;
            case ParagraphAlignment::Center:
                center_radio_button->SetSelected();
                break;
            case ParagraphAlignment::Far:
                bottom_radio_button->SetSelected();
                break;
        }
    }


	void CreateWordWrapingOption() {

		auto word_wrap_check_box = Create<CheckBox>();
		word_wrap_check_box->SetText(L"Word wrap");
		word_wrap_check_box->SetIsChecked(text_box_->GetWordWrapping() != WordWrapping::NoWrap);
		word_wrap_check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
			text_box_->SetWordWrapping(check_box->IsChecked() ? WordWrapping::Wrap : WordWrapping::NoWrap);
		});
		options_container_->AddChild(word_wrap_check_box);
	}


	void CreateSelectionRangeOption() {

		auto container = Create<Control>();
		container->SetLayouter(GetHorizontalArrayLayouter());
		options_container_->AddChild(container);

		auto label = Create<Label>();
		label->SetText(L"Selection range:");
		container->AddChild(label);

		auto selection_position_text_box = Create<TextBox>();
		text_box_->GetSelectionChangeEvent().AddListener([selection_position_text_box](const std::shared_ptr<TextBox>& text_box) {
			TextRange selection_range = text_box->GetSelectionRange();
			selection_position_text_box->SetText(std::to_wstring(selection_range.index));
		});
		container->AddChild(selection_position_text_box);

		auto selection_length_text_box = Create<TextBox>();
		text_box_->GetSelectionChangeEvent().AddListener([selection_length_text_box](const std::shared_ptr<TextBox>& text_box) {
			TextRange selection_range = text_box->GetSelectionRange();
			selection_length_text_box->SetText(std::to_wstring(selection_range.length));
		});
		container->AddChild(selection_length_text_box);

		auto button = Create<Button>();
		button->SetText(L"OK");
		button->GetClickEvent().AddListener([this, selection_position_text_box, selection_length_text_box](const std::shared_ptr<ClickableControl>& button) {
			
			TextRange new_range;

			auto position_string = selection_position_text_box->GetText();
			new_range.index = wcstoul(position_string.c_str(), nullptr, 10);
			
			auto length_string = selection_length_text_box->GetText();
			new_range.length = wcstoul(length_string.c_str(), nullptr, 10);

			text_box_->SetSelectionRange(new_range);
		});
		container->AddChild(button);
	}


    void CreateAllowBeepOption() {

        auto allow_beep_check_box = Create<CheckBox>();
        allow_beep_check_box->SetText(L"Allow beep");
        allow_beep_check_box->SetIsChecked(text_box_->AllowBeep());
        allow_beep_check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
            text_box_->SetAllowBeep(check_box->IsChecked());
        });
        options_container_->AddChild(allow_beep_check_box);
    }


    void CreateAllowScrollOption() {

        auto container = Create<Control>();
        container->SetLayouter(GetHorizontalArrayLayouter());
        options_container_->AddChild(container);

        auto allow_vertical_scroll = Create<CheckBox>();
        allow_vertical_scroll->SetText(L"Allow vertical scroll");
        allow_vertical_scroll->SetIsChecked(text_box_scroll_container_->AllowVerticalScroll());
        allow_vertical_scroll->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
            text_box_scroll_container_->SetAllowVerticalScroll(check_box->IsChecked());
        });
        container->AddChild(allow_vertical_scroll);

        auto allow_horizontal_scroll = Create<CheckBox>();
        allow_horizontal_scroll->SetText(L"Allow horizontal scroll");
        allow_horizontal_scroll->SetIsChecked(text_box_scroll_container_->AllowHorizontalScroll());
        allow_horizontal_scroll->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
            text_box_scroll_container_->SetAllowHorizontalScroll(check_box->IsChecked());
        });
        container->AddChild(allow_horizontal_scroll);
    }


    void CreateAutoHideScrollbarOption() {

        auto check_box = Create<CheckBox>();
        check_box->SetText(L"Auto hide scroll bars");
        check_box->SetIsChecked(text_box_scroll_container_->AutoHideScrollBars());
        check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
            text_box_scroll_container_->SetAutoHideScrollBars(check_box->IsChecked());
        });
        options_container_->AddChild(check_box);
    }


    void CreateHorizontalScrollOptions() {

        auto container = Create<Control>();
        container->SetLayouter(GetHorizontalArrayLayouter());
        options_container_->AddChild(container);

        auto left_to_begin_button = Create<Button>();
        left_to_begin_button->SetText(L"To left begin");
        left_to_begin_button->GetClickEvent().AddListener([this](const std::shared_ptr<ClickableControl>&) {
            text_box_->ScrollLeftToBegin();
        });
        container->AddChild(left_to_begin_button);

        auto right_to_begin_button = Create<Button>();
        right_to_begin_button->SetText(L"To right end");
        right_to_begin_button->GetClickEvent().AddListener([this](const std::shared_ptr<ClickableControl>&) {
            text_box_->ScrollRightToEnd();
        });
        container->AddChild(right_to_begin_button);
    }


    void CreateVerticalScrollOptions() {

        auto container = Create<Control>();
        container->SetLayouter(GetHorizontalArrayLayouter());
        options_container_->AddChild(container);

        auto line_up_button = Create<Button>();
        line_up_button->SetText(L"Line up");
        line_up_button->GetClickEvent().AddListener([this](const std::shared_ptr<ClickableControl>&) {
            text_box_->ScrollUpByLine();
        });
        container->AddChild(line_up_button);

        auto line_down_button = Create<Button>();
        line_down_button->SetText(L"Line down");
        line_down_button->GetClickEvent().AddListener([this](const std::shared_ptr<ClickableControl>&) {
            text_box_->ScrollDownByLine();
        });
        container->AddChild(line_down_button);

        auto page_up_button = Create<Button>();
        page_up_button->SetText(L"Page up");
        page_up_button->GetClickEvent().AddListener([this](const std::shared_ptr<ClickableControl>&) {
            text_box_->ScrollUpByPage();
        });
        container->AddChild(page_up_button);

        auto page_down_button = Create<Button>();
        page_down_button->SetText(L"Page down");
        page_down_button->GetClickEvent().AddListener([this](const std::shared_ptr<ClickableControl>&) {
            text_box_->ScrollDownByPage();
        });
        container->AddChild(page_down_button);

        auto to_begin_button = Create<Button>();
        to_begin_button->SetText(L"To begin");
        to_begin_button->GetClickEvent().AddListener([this](const std::shared_ptr<ClickableControl>&) {
            text_box_->ScrollUpToBegin();
        });
        container->AddChild(to_begin_button);

        auto to_end_button = Create<Button>();
        to_end_button->SetText(L"To end");
        to_end_button->GetClickEvent().AddListener([this](const std::shared_ptr<ClickableControl>&) {
            text_box_->ScrollDownToEnd();
        });
        container->AddChild(to_end_button);
    }


    void CreateUndoOptions() {

        auto container = Create<Control>();
        container->SetLayouter(GetHorizontalArrayLayouter());
        options_container_->AddChild(container);

        auto undo_button = Create<Button>();
        undo_button->SetText(L"Undo");
        undo_button->GetClickEvent().AddListener([this](const std::shared_ptr<ClickableControl>&) {
            text_box_->Undo();
        });
        container->AddChild(undo_button);

        auto redo_button = Create<Button>();
        redo_button->SetText(L"Redo");
        redo_button->GetClickEvent().AddListener([this](const std::shared_ptr<ClickableControl>&) {
            text_box_->Redo();
        });
        container->AddChild(redo_button);

        auto can_undo_check_box = Create<CheckBox>();
        can_undo_check_box->SetText(L"Can undo");
        can_undo_check_box->SetIsEnabled(false);
        can_undo_check_box->SetIsChecked(text_box_->CanUndo());
        container->AddChild(can_undo_check_box);

        auto can_redo_check_box = Create<CheckBox>();
        can_redo_check_box->SetText(L"Can redo");
        can_redo_check_box->SetIsEnabled(false);
        can_redo_check_box->SetIsChecked(text_box_->CanRedo());
        container->AddChild(can_redo_check_box);

        text_box_->GetTextChangeEvent().AddListener([can_undo_check_box, can_redo_check_box](const std::shared_ptr<TextBox>& text_box) {
            can_undo_check_box->SetIsChecked(text_box->CanUndo());
            can_redo_check_box->SetIsChecked(text_box->CanRedo());
        });
    }


    void CreateNumberOnlyOptions() {

        auto check_box = Create<CheckBox>();
        check_box->SetText(L"Numbers only");
        check_box->SetIsChecked(false);
        check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<CheckBox>& check_box) {
            text_box_->SetTextValidator(check_box->IsChecked() ? GetNumberTextValidator() : nullptr);
        });
        options_container_->AddChild(check_box);
    }

private:
	std::shared_ptr<Control> text_box_container_;
	std::shared_ptr<Control> options_container_;
    std::shared_ptr<ScrollableControl> text_box_scroll_container_;
	std::shared_ptr<TextBox> text_box_;
};


void ShowTestTextBoxWindow() {

	auto window = Create<TestTextBoxWindow>();
	window->Show();
}
