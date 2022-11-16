#pragma once

#include <zaf/control/list_control.h>

namespace zaf {

class ListBoxDataSource;
class ListBoxDelegate;

/**
 Represents a list control to display texts.   
 */
class ListBox : public ListControl {
public:
    ZAF_DECLARE_TYPE

public:
    ListBox();
    ~ListBox();

    /**
     Get item height.

     The default height is 18.
     */
    float GetDefaultItemHeight() const {
        return default_item_height_;
    }

    /**
     Set item height. 
     */
    void SetDefaultItemHeight(float item_height);

    void AddItem(const std::shared_ptr<Object>& data);
    void AddItemAtIndex(std::size_t index, const std::shared_ptr<Object>& data);
    void RemoveItemAtIndex(std::size_t index);
    void RemoveAllItems();

protected:
    void Initialize() override;
    void DataSourceChange(const std::shared_ptr<ListDataSource>& previous_data_source) override;
    void DelegateChange(const std::shared_ptr<ListControlDelegate>& previous_delegate) override;

private:
    std::shared_ptr<ListBoxDataSource> data_source_;
    std::shared_ptr<ListBoxDelegate> delegate_{};

    float default_item_height_{ 18 };
};

}