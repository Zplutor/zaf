#include "test_list_control_window.h"
#include <zaf/base/string/to_numeric.h>
#include <zaf/control/button.h>
#include <zaf/control/check_box.h>
#include <zaf/control/label.h>
#include <zaf/control/layout/array_layouter.h>
#include <zaf/control/list_control.h>
#include <zaf/control/radio_button.h>
#include <zaf/control/text_box.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

using namespace zaf;

class Item : public ListControl::Item {
public:
    
};

class ItemSource : public ListControl::ItemSource {
public:
    ItemSource() : seed_(0) {

        for (; seed_ < 100; ++seed_) {
            values_.push_back(std::make_pair(seed_, CreateVariableItemHeight(seed_)));
        }
    }

    std::size_t GetItemCount() override {
        return values_.size();
    }

    bool HasVariableItemHeight() override {
        return has_variable_item_height_;
    }

    float GetItemHeight(std::size_t index) override {
        return values_[index].second;
    }

    std::shared_ptr<ListControl::Item> CreateItem(std::size_t index) override {
        auto item = Create<Item>();
        item->SetText(std::to_wstring(values_[index].first));
        return item;
    }

    void SetHasVariableItemHeight(bool value) {
        has_variable_item_height_ = value;
    }

    void AddItem(std::size_t index, std::size_t count) {

        if (index > values_.size()) {
            return;
        }

        auto insert_iterator = std::next(values_.begin(), index);
        values_.insert(insert_iterator, count, std::pair<std::size_t, float>());

        for (std::size_t current_index = index; current_index < index + count; ++current_index) {
            values_[current_index].first = seed_++;
            values_[current_index].second = CreateVariableItemHeight(current_index);
        }

        NotifyItemAdd(index, count);
    }

    void RemoveItem(std::size_t index, std::size_t count) {

        if (index >= values_.size()) {
            return;
        }

        if (count > values_.size() - index) {
            return;
        }

        auto erase_iterator = std::next(values_.begin(), index);
        values_.erase(erase_iterator, std::next(erase_iterator, count));

        NotifyItemRemove(index, count);
    }

    void UpdateItem(std::size_t index, std::size_t count) {

        if (index >= values_.size()) {
            return;
        }

        if (count > values_.size() - index) {
            return;
        }

        auto begin_iterator = values_.begin();
        auto end_iterator = std::next(begin_iterator, count);

        for (auto iterator = std::next(begin_iterator, index); 
             iterator != end_iterator;
             ++iterator) {

            iterator->first += 1;
            iterator->second = CreateVariableItemHeight(std::distance(begin_iterator, iterator));
        }

        NotifyItemUpdate(index, count);
    }

private:
    static float CreateVariableItemHeight(std::size_t index) {
        return static_cast<float>(index % 20 + 20);
    }

private:
    bool has_variable_item_height_ = false;
    std::size_t seed_;
    std::vector<std::pair<std::size_t, float>> values_;
};

class TestListControlWindow : public Window {
public:
    void Initialize() override {

        __super::Initialize();

        GetRootControl()->SetLayouter(GetHorizontalArrayLayouter());

        InitializeListControl();
        InitializeOptions();
    }

    void InitializeListControl() {

        item_source_ = std::make_shared<ItemSource>();
        list_control_ = Create<ListControl>();
        list_control_->SetItemSource(item_source_);
        GetRootControl()->AddChild(list_control_);
    }

    void InitializeOptions() {

        options_container_ = Create<Control>();
        options_container_->SetLayouter(GetVerticalArrayLayouter());
        GetRootControl()->AddChild(options_container_);

        InitializeSelectionOption();
        InitializeHasVariableItemHeightOption();
        InitializeSelectionModeOption();
        InitializeAddItemOption();
        InitializeRemoveItemOption();
        InitializeUpdateItemOption();
    }


    void InitializeSelectionOption() {

        auto label = Create<Label>();
        options_container_->AddChild(label);

        auto update_label_text = [this, label]() {
            label->SetText(
                L"First selected index: " + 
                std::to_wstring(list_control_->GetFirstSelectedItemIndex()) + 
                L"  Count: " +
                std::to_wstring(list_control_->GetSelectedItemCount()));
        };

        list_control_->GetSelectionChangeEvent().AddListener([update_label_text](const std::shared_ptr<ListControl>&) {
            update_label_text();
        });

        update_label_text();
    }


    void InitializeHasVariableItemHeightOption() {

        auto check_box = Create<CheckBox>();
        check_box->SetText(L"Has variable item height");
        check_box->SetIsChecked(item_source_->HasVariableItemHeight());

        check_box->GetCheckStateChangeEvent().AddListener([this](const std::shared_ptr<ClickableControl>&) {
            item_source_->SetHasVariableItemHeight(! item_source_->HasVariableItemHeight());
            list_control_->Reload();
        });

        options_container_->AddChild(check_box);
    }

    void InitializeSelectionModeOption() {

        auto container = Create<Control>();
        container->SetLayouter(GetHorizontalArrayLayouter());
        options_container_->AddChild(container);

        auto selection_mode_group = std::make_shared<RadioButton::Group>();

        struct {
            const wchar_t* const text;
            ListControl::SelectionMode selection_mode;
        } selection_mode_options[] = {
            L"None", ListControl::SelectionMode::None,
            L"Single", ListControl::SelectionMode::Single,
            L"Simple multiple", ListControl::SelectionMode::SimpleMultiple,
            L"Extended multiple", ListControl::SelectionMode::ExtendedMultiple,
        };

        for (const auto& each_option : selection_mode_options) {

            auto radio_button = Create<RadioButton>();
            radio_button->SetText(each_option.text);
            radio_button->SetGroup(selection_mode_group);
            
            if (list_control_->GetSelectionMode() == each_option.selection_mode) {
                radio_button->SetSelected();
            }

            radio_button->GetSelectStateChangeEvent().AddListener([this, each_option](const std::shared_ptr<RadioButton>& radio_button) {
                if (radio_button->IsSelected()) {
                    list_control_->SetSelectionMode(each_option.selection_mode);
                }
            });

            container->AddChild(radio_button);
        }
    }

    void InitializeAddItemOption() {

        auto container = Create<Control>();
        container->SetLayouter(GetHorizontalArrayLayouter());
        options_container_->AddChild(container);

        auto index_text_box = Create<TextBox>();
        container->AddChild(index_text_box);

        auto count_text_box = Create<TextBox>();
        container->AddChild(count_text_box);

        auto add_button = Create<Button>();
        add_button->SetText(L"Add");
        add_button->GetClickEvent().AddListener([this, index_text_box, count_text_box](const std::shared_ptr<ClickableControl>&) {

            std::size_t index = 0;
            if (! TryToNumeric<std::size_t>(index_text_box->GetText(), index)) {
                return;
            }

            std::size_t count = 0;
            if (! TryToNumeric<std::size_t>(count_text_box->GetText(), count)) {
                return;
            }

            item_source_->AddItem(index, count);
        });
        container->AddChild(add_button);
    }

    void InitializeRemoveItemOption() {

        auto container = Create<Control>();
        container->SetLayouter(GetHorizontalArrayLayouter());
        options_container_->AddChild(container);

        auto index_text_box = Create<TextBox>();
        container->AddChild(index_text_box);

        auto count_text_box = Create<TextBox>();
        container->AddChild(count_text_box);

        auto remove_button = Create<Button>();
        remove_button->SetText(L"Remove");
        remove_button->GetClickEvent().AddListener([this, index_text_box, count_text_box](const std::shared_ptr<ClickableControl>&) {

            std::size_t index = 0;
            if (!TryToNumeric<std::size_t>(index_text_box->GetText(), index)) {
                return;
            }

            std::size_t count = 0;
            if (!TryToNumeric<std::size_t>(count_text_box->GetText(), count)) {
                return;
            }

            item_source_->RemoveItem(index, count);
        });
        container->AddChild(remove_button);
    }
    
    void InitializeUpdateItemOption() {

        auto container = Create<Control>();
        container->SetLayouter(GetHorizontalArrayLayouter());
        options_container_->AddChild(container);

        auto index_text_box = Create<TextBox>();
        container->AddChild(index_text_box);

        auto count_text_box = Create<TextBox>();
        container->AddChild(count_text_box);

        auto update_button = Create<Button>();
        update_button->SetText(L"Update");
        update_button->GetClickEvent().AddListener([this, index_text_box, count_text_box](const std::shared_ptr<ClickableControl>&) {

            std::size_t index = 0;
            if (!TryToNumeric<std::size_t>(index_text_box->GetText(), index)) {
                return;
            }

            std::size_t count = 0;
            if (!TryToNumeric<std::size_t>(count_text_box->GetText(), count)) {
                return;
            }

            item_source_->UpdateItem(index, count);
        });
        container->AddChild(update_button);
    }

private:
    std::shared_ptr<ListControl> list_control_;
    std::shared_ptr<ItemSource> item_source_;
    std::shared_ptr<Control> options_container_;
};

void ShowTestListControlWindow() {

    auto window = Create<TestListControlWindow>();
    window->Show();
}