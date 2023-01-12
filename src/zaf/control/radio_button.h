#pragma once 

#include <zaf/control/clickable_control.h>
#include <zaf/rx/observable.h>

namespace zaf{

class RadioButtonSelectStateChangeInfo;

/**
 Represents a radio button conrol.    
 */
class RadioButton : public ClickableControl {
public:
    ZAF_DECLARE_TYPE

public:
    /**
     Represents a radio button group.

     Radio buttons associate with the same group cannot be selected at the same time.
     Use RadioButton::SetGroup method to associate a radio buton with a group.
     */
    class Group {
    public:
        /**
         Get associated radio buttons.
         */
        const std::vector<std::shared_ptr<RadioButton>> GetRadioButtons() const;

    private:
        friend class RadioButton;

        void Add(const std::shared_ptr<RadioButton>& radio_button);
        void Remove(const std::shared_ptr<RadioButton>& radio_button);

        void RadioButtonSelected(const std::shared_ptr<RadioButton>& radio_button);

    private:
        std::vector<std::weak_ptr<RadioButton>> radio_buttons_;
    };

public:
    RadioButton();
    ~RadioButton();

    /**
     Get radio border color.
     */
    Color GetRadioBorderColor() const {
        return GetRadioBorderColorPicker()(*this);
    }

    /**
     Get the color picker of radio border.
     */
    ColorPicker GetRadioBorderColorPicker() const;

    /**
     Set radio border color.
     */
    void SetRadioBorderColor(const Color& color) {
        SetRadioBorderColorPicker(CreateColorPicker(color));
    }

    /**
     Set the color picker of radio border.
     */
    void SetRadioBorderColorPicker(const ColorPicker& color_picker);

    /**
     Get radio background color.
     */
    Color GetRadioBackgroundColor() {
        return GetRadioBackgroundColorPicker()(*this);
    }

    /**
     Get the color picker of radio background.
     */
    ColorPicker GetRadioBackgroundColorPicker() const;

    /**
     Set radio background color.
     */
    void SetRadioBackgroundColor(const Color& color) {
        SetRadioBackgroundColorPicker(CreateColorPicker(color));
    }

    /**
     Set the color picker of radio background.
     */
    void SetRadioBackgroundColorPicker(const ColorPicker& color_picker);

    /**
     Get the associated group.

     @return
        Return nullptr if there is no associated group.
     */
    const std::shared_ptr<Group>& GetGroup() const {
        return group_;
    }

    /**
     Set the associated group.

     The first radio button associates with a group would be selected.
     The later associated radio buttons would be unselected.

     If group is nullptr, the association would be removed.
     */
    void SetGroup(const std::shared_ptr<Group>& group);

    /**
     Get a value indicating whether the radio button changes its select state automatically 
     when it is clicked.

     The default value is true.
     */
    bool CanAutoSelect() const;

    /**
     Set a value indicating whether the radio button changes its select state automatically 
     when it is clicked.
     */
    void SetCanAutoSelect(bool can_auto_select);

    /**
     Get a value indicating whether the radio button is selected.
     */
    bool IsSelected() const {
        return is_selected_;
    }

    /**
     Set the radio button selected.

     Other radio buttons associate with the same group would be unselected.
     */
    void SetSelected() {
        SetIsSelected(true);
    }

    /**
     Get the select state change event.
     */
    Observable<RadioButtonSelectStateChangeInfo> SelectStateChangeEvent();

protected:
    void Initialize() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) override;
    zaf::Rect GetTextRect() override;
    void OnClick(const ClickInfo& event_info) override;

private:
    void PaintRadio(Canvas& canvas, const zaf::Rect& radio_rect);

    void SetUnselected() {
        SetIsSelected(false);
    }

    void SetIsSelected(bool is_selected);

private:
    bool is_selected_;
    std::shared_ptr<Group> group_;
};


class RadioButtonSelectStateChangeInfo {
public:
    RadioButtonSelectStateChangeInfo(const std::shared_ptr<RadioButton>& radio_button) :
        radio_button_(radio_button) {

    }

    const std::shared_ptr<RadioButton>& RadioButton() const {
        return radio_button_;
    }

private:
    std::shared_ptr<zaf::RadioButton> radio_button_;
};

}